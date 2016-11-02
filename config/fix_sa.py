#!/usr/bin/env python2

from gta3sc import *
from collections import defaultdict, namedtuple
from itertools import chain

# WARNING TEXT_LABEL AllowGlobal AllowLocal = false
# TODO ADD ENUM TO LOAD_MISSION_AUDIO
# FIND NOPS and remove arguments from them and do supported=false
# add text limits
# CREATE_PLAYER doesn't have a CARPEDMODEL enum
# check all MODEL if not actually CARPEDMODEL
# enums from desc; NOTE do not add enums in Out parameters
# entity by desc
# plane heli train derived from car
# SCRIPT_EVENT


def print_once(iterator):
    printed = set()
    for x in iterator:
        if x not in printed:
            printed.add(x)
            print(x)

def copy_properties(gta3_commands, gtavc_commands, gtasa_commands):
    for cmdid, sacmd in gtasa_commands.iteritems():
        print("Copying properties for %s:%s..." % (hex(cmdid), sacmd.name))
        vccmd = gtavc_commands.get(cmdid)
        g3cmd = gtavc_commands.get(cmdid)
        if vccmd is not None and sacmd.same_behaviour(vccmd):
            #print(hex(cmdid), vccmd.name, sacmd.name)     
            assert vccmd.name == sacmd.name or vccmd.name in ('SWITCH_HELICOPTER', 'SET_CAR_CHANGE_LANE')

            for s,v in zip(sacmd.args, vccmd.args):
                assert s.ref == v.ref
                assert s.out == v.out
                assert s.entity == None or s.entity == v.entity
                assert len(s.enums) == 0 or s.enums == v.enums
                assert s.allow_const or s.allow_const == v.allow_const
                assert s.allow_gvar or s.allow_gvar == v.allow_gvar
                assert s.allow_lvar or s.allow_lvar == v.allow_lvar
                s.allow_const = v.allow_const
                s.allow_gvar  = v.allow_gvar
                s.allow_lvar  = v.allow_lvar
                s.entity      = v.entity
                s.enums       = v.enums

        elif g3cmd is not None and sacmd.same_behaviour(g3cmd):
            assert False
        else:
            if len(sacmd.args) == 0 and not sacmd.supported:
                pass
            elif sacmd.id > 0x5a8:
                #print(sacmd.name)
                pass
            else:
                #print(sacmd.name)
                #could add a sacmd.name in ('',...) check
                pass

def commands_with_model_args(commands):
    for cmd in commands.itervalues():
        for arg in cmd.args:
            if any(arg.has_enum("MODEL") for arg in cmd.args):
                yield cmd.name

def argument_descriptions(commands):
    for cmd in commands.itervalues():
        for arg in cmd.args:
            yield arg.desc

def discover_properties_from_description(commands):

    temp_all_descs = list(argument_descriptions(commands))
    assert len(set(temp_all_descs)) == len(set(x.upper() for x in temp_all_descs))
    del temp_all_descs

    Enum = namedtuple('Enum', ['defined_at', 'name'])
    Entity = namedtuple('Entity', ['defined_at', 'name'])

    dictionary = {}
    used_in_cmds = defaultdict(list)
    for cmd in commands.itervalues():
        for arg in cmd.args:
            if arg.desc:
                if arg.desc in ("Model ID",):
                    continue
                used_in_cmds[arg.desc].append(cmd)
                mydata = dictionary.get(arg.desc)
                newdata = None
                try:
                    if mydata is None:
                        if len(arg.enums) > 0:
                            newdata = Enum(cmd, arg.enums[0])
                            assert len(arg.enums) == 1
                            assert arg.entity is None
                        if arg.entity is not None:
                            newdata = Entity(cmd, arg.entity)
                            assert len(arg.enums) == 0
                        if newdata is not None:
                            mydata = newdata
                            dictionary[arg.desc] = newdata
                    elif isinstance(mydata, Enum):
                        assert arg.entity is None
                        if len(arg.enums) > 0:
                            assert len(arg.enums) == 1
                            assert mydata.name == arg.enums[0]
                    elif isinstance(mydata, Entity):
                        assert len(arg.enums) == 0
                        if arg.entity is not None:
                            assert mydata.name == arg.entity
                except AssertionError: # HACKY
                    print("======= PROPERTIES CONFLICT ==========")
                    print("Argument Description: %s" % (arg.desc))
                    if mydata is not None:
                        print("Previous property found in %s with value %s and type %s." % (mydata.defined_at.name, mydata.name, type(mydata).__name__))
                    else:
                        print("Previous data never found.")
                    if newdata is not None:
                        print("Currently parsing data is in %s with value %s and type %s" % (newdata.defined_at.name, newdata.name, type(newdata).__name__))
                    else:
                        print("Currently parsing data is in %s" % (cmd.name))
                    print("=====================================")
                    raise 

    #print(dictionary.get("Boolean true/false").defined_at.name)
    assert dictionary.get("Model ID") == None
    assert dictionary.get("Boolean true/false") == None

    for desc, data in dictionary.iteritems():
        print("%s: %s of type %s" % (desc, data.name, type(data).__name__))
        for cmd in used_in_cmds.get(desc):
            print("    %s" % (cmd.name))
        print("")

    for cmd in commands.itervalues():
        for arg in cmd.args:
            data = dictionary.get(arg.desc)
            if data is None:
                pass
            elif isinstance(data, Enum):
                if not arg.enums:
                    arg.enums.append(data.name)
            elif isinstance(data, Entity):
                if not arg.entity:
                    arg.entity = data.name

def find_missing_properties_from_command_name(commands_notfiltered):

    entity_types = set()
    enum_types = set()
    for cmd in commands_notfiltered.itervalues():
        for arg in cmd.args:
            if arg.enums:  enum_types.add(arg.enums[0])
            if arg.entity: entity_types.add(arg.entity)

    commands = {c.id: c for c in commands_notfiltered.itervalues() if c.supported}

    entity_missing = defaultdict(list)
    enum_missing = defaultdict(list)

    for cmd in commands.itervalues():
        for name in entity_types:
            if name in cmd.name:
                if not any(a.entity == name for a in cmd.args):
                    entity_missing[name].append(cmd)

    for cmd in commands.itervalues():
        for name in enum_types:
            if name in cmd.name:
                if not any(a.enums and a.enums[0] == name for a in cmd.args):
                    enum_missing[name].append(cmd)

    print("============================================")
    print("Entities Missing")
    print("============================================")
    for name, cmdlist in entity_missing.iteritems():
        print("    %s" % (name))
        for cmd in cmdlist:
            print("        %s" % (cmd.name))
        print("")

    print("============================================")
    print("Enums Missing")
    print("============================================")
    for name, cmdlist in enum_missing.iteritems():
        print("    %s" % (name))
        for cmd in cmdlist:
            print("        %s" % (cmd.name))
        print("")





def main():
    gtasa_commands = {c.id: c for c in commands_from_xml("gtasa/commands.xml")}
    gtavc_commands = {c.id: c for c in commands_from_xml("gtavc/commands.xml")}
    gta3_commands  = {c.id: c for c in commands_from_xml("gta3/commands.xml")}

    #find_missing_properties_from_command_name(gtasa_commands)

    nops = [
#0x03A9,
0x0666,
0x066F,
0x0670,
0x0671,
0x0675,
0x0678,
0x068F,
0x069C,
0x069D,
0x069E,
0x069F,
0x06A0,
0x06A1,
0x06A4,
0x06A6,
0x06C6,
0x06CB,
0x06CE,
0x06F4,
0x06F6,
0x06F7,
0x06F9,
0x06FB,
0x06FE,
0x0700,
0x0711,
0x0712,
0x0718,
0x071B,
0x071C,
0x071D,
0x0720,
0x0721,
0x0722,
0x0725,
0x0728,
0x0738,
0x0739,
0x073A,
0x073D,
0x0740,
0x0744,
0x0748,
0x0758,
0x0759,
0x0764,
0x0765,
0x0766,
0x076E,
0x0773,
0x0774,
0x0775,
0x0779,
0x077F,
0x0785,
0x0787,
0x078D,
0x078E,
0x0791,
0x0795,
0x079A,
0x079B,
0x079C,
0x07A2,
0x07AA,
0x07AD,
0x07AE,
0x07B2,
0x07B5,
0x07B6,
0x07B7,
0x07B8,
0x07BA,
0x07C2,
0x07C8,
0x07CA,
0x07CE,
0x07CF,
0x07D1,
0x07D2,
0x07D4,
0x07DC,
0x07E2,
0x07E3,
0x07E9,
0x07EA,
0x07EB,
0x07EC,
0x07ED,
0x07F4,
0x0805,
0x0807,
0x0808,
0x0809,
0x080C,
0x080D,
0x080F,
0x0813,
0x081B,
0x0821,
0x0824,
0x0831,
0x0832,
0x0838,
0x083B,
0x0848,
0x0849,
0x084A,
0x084B,
0x084C,
0x084F,
0x0857,
0x085C,
0x085D,
0x085F,
0x0862,
0x0863,
0x086A,
0x086D,
0x086E,
0x086F,
0x0870,
0x0877,
0x0880,
0x0882,
0x0885,
0x0886,
0x088F,
0x0891,
0x0892,
0x0894,
0x0895,
0x0896,
0x0899,
0x089A,
0x089D,
0x08A1,
0x08AA,
0x08AE,
0x08B0,
0x08CC,
0x08CD,
0x08CE,
0x08CF,
0x08D5,
0x08FC,
0x0902,
0x0903,
0x090A,
0x090B,
0x0911,
0x091B,
0x0921,
0x0927,
0x092A,
0x092C,
0x092D,
0x0932,
0x0935,
0x0938,
0x093C,
0x093F,
0x0943,
0x0962,
0x0963,
0x0979,
0x097E,
0x097F,
0x098F,
0x0990,
0x0993,
0x0999,
0x09A5,
0x09B1,
0x09C6,
0x09D3,
0x09DC,
0x09EA,
0x09F3,
0x09F9,
0x0A00,
0x0A04,
0x0A05,
0x0A0D,
0x0A42,
    ]
    for cmd in gtasa_commands.itervalues():
        if cmd.id in nops:
            if len(cmd.args) > 0 or cmd.supported:
                print(cmd.name)
                cmd.args = []
                cmd.supported = False

    commands_to_xml("gtasa/commands.xml", [c for c in gtasa_commands.itervalues()])



if __name__ == "__main__":
    main()


