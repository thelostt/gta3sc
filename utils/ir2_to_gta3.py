#!/usr/bin/env python2
"""
  Examples:
    py ir2_to_gta3.py main.ir2 ../config/gta3
"""
import sys, os, errno
import gta3sc
from gta3sc.bytecode import Scope
from gta3sc.bytecode import DATATYPE_GLOBALVAR_NUMBER
from gta3sc.bytecode import DATATYPE_GLOBALVAR_TEXTLABEL
from gta3sc.bytecode import DATATYPE_GLOBALVAR_TEXTLABEL16
from gta3sc.bytecode import BYTECODE_OFFSET_MAIN
from gta3sc.bytecode import BYTECODE_OFFSET_MISSION
from gta3sc.bytecode import BYTECODE_OFFSET_STREAMED
from collections import defaultdict

TIMER_INDICES = (16, 17)

def converted_arg(ir2, arg, arginfo, enums=None):
    if arg.is_number():
        if arg.is_float():
            output = "%f" % arg.value
            output = output.rstrip("0")
            if output.endswith("."):
                output += "0"
            return output
        else:
            if len(arginfo.enums) > 0:
                if arginfo.enums[0] == "MODEL":
                    if arg.value < 0:
                        return ir2.get_model(-arg.value - 1) or str(arg.value)
                    else:
                        return enums["CARPEDMODEL"].get(arg.value) or str(arg.value)
                else:
                    enum = enums.get(arginfo.enums[0])
                    if enum != None:
                        return enum.get(arg.value, str(arg.value))
            elif arginfo.desc.startswith("Boolean") and arg.value in (0,1):
                return ("FALSE", "TRUE")[arg.value]
            return str(arg.value)
    elif arg.is_label():
        return arg.value.lower()
    elif arg.is_string():
        # TODO escaping and quoted strings?
        if arg.is_buffer128():
            return '"%s"' % arg.value
        else:
            return arg.value.upper()
    elif arg.is_var():
        if arg.is_array():
            return "%s[%s]" % (converted_arg(arg.base), converted_arg(arg.index))
        elif arg.is_local() and arg.offset in (TIMER_INDICES[0] * 4, TIMER_INDICES[1] * 4):
            if arg.offset == TIMER_INDICES[0] * 4:
                return "timera"
            else:
                return "timerb"
        else:
            assert (arg.offset % 4) == 0
            index = arg.offset / 4
            prefix = 'l' if arg.is_local() else ''
            if arg.get_datatype() in (DATATYPE_GLOBALVAR_TEXTLABEL, DATATYPE_GLOBALVAR_TEXTLABEL16):
                assert arginfo.optional == False
                return ("$%svar_%s" if arginfo.allow_const else "%svar_%s") % (prefix, index)
            else:
                return "%svar_%s" % (prefix, index)
    else:
        assert False

def converted_expr(ir2, data, cmdinfo, op):
    arg0 = converted_arg(ir2, data.args[0], cmdinfo.get_arg(0))
    arg1 = converted_arg(ir2, data.args[1], cmdinfo.get_arg(1))
    return "%s%s %s %s" % ("NOT " if data.not_flag else "", arg0, op, arg1)

def converted_data(ir2, data, commands, alternators, enums, subscripts=None, gosubfiles=None, alternative_name=None):
    if data.is_label():
        return "%s:" % data.name
    elif data.is_command():
        cmdinfo = commands[data.name]
        output = "NOT " if data.not_flag else ""
        cmdname = data.name if not alternative_name else alternative_name
        if cmdname in alternators["SET"]:
            return converted_expr(ir2, data, cmdinfo, '=')
        elif cmdname in alternators["CSET"]:
            return converted_expr(ir2, data, cmdinfo, '=#')
        elif cmdname in alternators["ADD_THING_TO_THING"]:
            return converted_expr(ir2, data, cmdinfo, '+=')
        elif cmdname in alternators["SUB_THING_FROM_THING"]:
            return converted_expr(ir2, data, cmdinfo, '-=')
        elif cmdname in alternators["MULT_THING_FROM_THING"]:
            return converted_expr(ir2, data, cmdinfo, '*=')
        elif cmdname in alternators["DIV_THING_BY_THING"]:
            return converted_expr(ir2, data, cmdinfo, '/=')
        elif cmdname in alternators["IS_THING_GREATER_THAN_THING"]:
            return converted_expr(ir2, data, cmdinfo, '>')
        elif cmdname in alternators["IS_THING_GREATER_OR_EQUAL_TO_THING"]:
            return converted_expr(ir2, data, cmdinfo, '>=')
        elif cmdname in alternators["ADD_THING_TO_THING_TIMED"]:
            return converted_expr(ir2, data, cmdinfo, '+=@')
        elif cmdname in alternators["SUB_THING_FROM_THING_TIMED"]:
            return converted_expr(ir2, data, cmdinfo, '-=@')
        elif cmdname in alternators["IS_THING_EQUAL_TO_THING"]:
            return converted_data(ir2, data, commands, alternators, enums, alternative_name='IS_THING_EQUAL_TO_THING')
        elif cmdname in alternators["ABS"]:
            return converted_data(ir2, data, commands, alternators, enums, alternative_name='ABS')
        elif cmdname in alternators["IS_BIT_SET"]:
            return converted_data(ir2, data, commands, alternators, enums, alternative_name='IS_BIT_SET')
        elif cmdname in alternators["SET_BIT"]:
            return converted_data(ir2, data, commands, alternators, enums, alternative_name='SET_BIT')
        elif cmdname in alternators["CLEAR_BIT"]:
            return converted_data(ir2, data, commands, alternators, enums, alternative_name='CLEAR_BIT')
        elif cmdname in alternators["IS_EMPTY"]:
            return converted_data(ir2, data, commands, alternators, enums, alternative_name='IS_EMPTY')
        elif cmdname in ("SET_TOTAL_NUMBER_OF_MISSIONS", "SET_PROGRESS_TOTAL", "SET_COLLECTABLE1_TOTAL"):
            return "%s 0" % (cmdname)
        elif cmdname == "GOSUB_FILE":
            assert data.args[1].is_label()
            arg1 = gosubfiles[ir2.offset_from_label(data.args[1].value)]
            arg0 = converted_arg(ir2, data.args[0], cmdinfo.get_arg(0), enums)
            return "GOSUB_FILE %s %s" % (arg0, arg1)
        elif cmdname == "LAUNCH_MISSION":
            assert data.args[0].is_label()
            arg0 = subscripts[ir2.offset_from_label(data.args[0].value)]
            return "LAUNCH_MISSION %s" % (arg0)
        elif cmdname == "LOAD_AND_LAUNCH_MISSION_INTERNAL":
            assert data.args[0].is_number()
            mission_offset = ir2.offset_from_mission(data.args[0].value)
            return "LOAD_AND_LAUNCH_MISSION %s" % (filename_from_offset(mission_offset))
        elif cmdname == "REGISTER_STREAMED_SCRIPT_INTERNAL":
            assert data.args[0].is_number()
            streamed_offset = ir2.offset_from_mission(data.args[0].value)
            return "REGISTER_STREAMED_SCRIPT %s" % (filename_from_offset(streamed_offset))
        else:
            output += cmdname
            for i, arg in enumerate(data.args):
                output += " %s" % converted_arg(ir2, arg, cmdinfo.get_arg(i), enums)
            return output
    else:
        assert False

def filename_from_offset(offset):
    if offset.type == BYTECODE_OFFSET_MISSION:
        return "mission%d.sc" % offset.block
    elif offset.type == BYTECODE_OFFSET_STREAMED:
        return "stream%d.sc" % offset.block
    else:
        assert False

def print_vars(stream, vars, is_local, tab=0):
    any_var = False
    last_var_index = -1 if is_local else 1
    pfx = 'l' if is_local else ''
    pfxu = (' ' * (tab*4)) + pfx.upper()
    for v in vars:
        this_var_index = v.start_offset / 4
        if is_local and this_var_index >= TIMER_INDICES[0]:
            break
        any_var = True
        for k in range(last_var_index + 1, this_var_index):
            stream.write("%sVAR_INT %svar_%d // unused variable\n" % (pfxu, pfx, k))
        comment = "// unknown type" if not v.type else ""
        if v.size == None:
            stream.write("%sVAR_%s %svar_%d%s\n" % (pfxu, v.type or "INT", pfx, this_var_index, comment))
        else:
            stream.write("%sVAR_%s %svar_%d[%d]%s\n" % (pfxu, v.type, pfx, this_var_index, v.size, comment))
        last_var_index = this_var_index
    if any_var:
        stream.write("\n\n")

def main(ir2file, xmlfile):
    config = gta3sc.read_config(xmlfile)
    ir2 = gta3sc.read_ir2(ir2file)

    scopes_before_label = False
    split_files = True
    output_dir = "test"

    commands    = {cmd.name: cmd for cmd in config.commands}
    alternators = defaultdict(set, {alt.name: set(alt.alters) for alt in config.alternators})
    enums       = {enum.name: {v: k for k,v in enum.constants.iteritems()} for enum in config.enums}

    scopes = ir2.discover_scopes()
    subscripts = dict()
    gosubfiles = dict()
    current_scope = None
    first_scope = scopes[0] if len(scopes) > 0 else None

    for off, data in ir2:
        if data.is_command() and data.name == "LAUNCH_MISSION":
            assert data.args[0].is_label()
            filename = "subscript%d.sc" % len(subscripts)
            subscripts[ir2.offset_from_label(data.args[0].value)] = filename
        elif data.is_command() and data.name == "GOSUB_FILE":
            assert data.args[1].is_label()
            filename = "gosub%d.sc" % len(gosubfiles)
            gosubfiles[ir2.offset_from_label(data.args[1].value)] = filename

    global_vars = ir2.discover_global_vars(commands=commands)

    print("//--------------------------")

    try:
        os.makedirs(os.path.join(output_dir, "main"))
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise

    stream = sys.stdout if not split_files else open(os.path.join(output_dir, "main.sc"), 'w')

    got_mission_terminate = [None] # hack

    print_vars(stream, global_vars, False)

    for off, data in ir2:

        def on_scope_begin(old_scope, new_scope):
            if new_scope.start in subscripts or (old_scope.start.type != new_scope.start.type or old_scope.start.block != new_scope.start.block):
                if new_scope.start.type != BYTECODE_OFFSET_MAIN or new_scope.start in subscripts:
                    stream.write("%s\n" % ("MISSION_START", "MISSION_START", "SCRIPT_START")[new_scope.start.type])
                    got_mission_terminate[0] = False
        def on_scope_end(old_scope, new_scope):
            if old_scope.start in subscripts or (old_scope.start.type != new_scope.start.type or old_scope.start.block != new_scope.start.block):
                if old_scope.start.type != BYTECODE_OFFSET_MAIN or old_scope.start in subscripts:
                    #stream.write("%s\n" % ("MISSION_END", "MISSION_END", "SCRIPT_END")[old_scope.start.type])
                    got_mission_terminate[0] = None

        def write_data(tab=0):
            tabing = ' ' * (tab*4)
            if data.is_label(): stream.write("\n")
            line = converted_data(ir2, data, commands, alternators, enums, subscripts=subscripts, gosubfiles=gosubfiles)
            stream.write("%s%s\n" % (tabing, line))

        if current_scope == None:
            if first_scope != None and off >= first_scope.start:
                current_scope = Scope.from_offset(off, scopes)
                assert current_scope != None
        elif not current_scope.owns_offset(off):
            previous_scope = current_scope
            if current_scope != first_scope:
                stream.write("}\n")
            current_scope = Scope.from_offset(off, scopes)
            assert current_scope != None

            on_scope_end(previous_scope, current_scope)

            if split_files:
                if off.type != BYTECODE_OFFSET_MAIN:
                    stream.close()
                    stream = open(os.path.join(output_dir, "main", filename_from_offset(off)), 'w')
                elif off in subscripts or off in gosubfiles:
                    filename = subscripts.get(off) or gosubfiles.get(off)
                    stream.close()
                    stream = open(os.path.join(output_dir, "main", filename), 'w')

            on_scope_begin(previous_scope, current_scope)
            local_vars = ir2.discover_local_vars(current_scope, commands=commands)

            if data.is_label():
                if scopes_before_label:
                    stream.write("\n{")
                    write_data(tab=1)
                    print_vars(stream, local_vars, True, tab=1)
                    continue
                else:
                    write_data(tab=0)
                    stream.write("{\n")
                    print_vars(stream, local_vars, True, tab=1)
                    continue
            else:
                stream.write("{\n")
                print_vars(stream, local_vars, True, tab=1)

        if got_mission_terminate[0] == False and\
           data.is_command() and data.name == "TERMINATE_THIS_SCRIPT":
            stream.write("    MISSION_END\n\n\n")
            got_mission_terminate[0] = True
        else:
            tab = int(current_scope != None and current_scope != first_scope)
            if got_mission_terminate[0] == False: tab += 1
            write_data(tab=tab)

    if current_scope != None:
        current_scope = None
        stream.write("}")

    if stream != sys.stdout:
        stream.close()


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: discover_entity_commands.py <ir2_script> <xmlfile>")
        sys.exit(1)
    main(sys.argv[1], sys.argv[2])

