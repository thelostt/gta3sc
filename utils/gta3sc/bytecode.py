# -*- Python -*-

import re

__all__ = [
    "Bytecode", "Data", "Arg", "Label", "Hex", "Command", 
     "ArgNumber", "ArgLabel", "ArgString", "ArgVariable", "ArgArray",
     "read_ir2",
]

DATA_HEX     = 0
DATA_LABEL   = 1
DATA_COMMAND = 2

DATATYPE_INT8                         = 0
DATATYPE_INT16                        = 1
DATATYPE_INT32                        = 2
DATATYPE_LOCAL_LABEL                  = 3
DATATYPE_GLOBAL_LABEL                 = 4
DATATYPE_FLOAT                        = 5
DATATYPE_GLOBALVAR_NUMBER             = 6
DATATYPE_GLOBALVAR_TEXTLABEL          = 7
DATATYPE_GLOBALVAR_TEXTLABEL16        = 8
DATATYPE_LOCALVAR_NUMBER              = 9
DATATYPE_LOCALVAR_TEXTLABEL           = 10
DATATYPE_LOCALVAR_TEXTLABEL16         = 11
DATATYPE_GLOBALVAR_ARRAY_NUMBER       = 12
DATATYPE_GLOBALVAR_ARRAY_TEXTLABEL    = 13
DATATYPE_GLOBALVAR_ARRAY_TEXTLABEL16  = 14
DATATYPE_LOCALVAR_ARRAY_NUMBER        = 15
DATATYPE_LOCALVAR_ARRAY_TEXTLABEL     = 16
DATATYPE_LOCALVAR_ARRAY_TEXTLABEL16   = 17
DATATYPE_TEXTLABEL8                   = 18
DATATYPE_TEXTLABEL16                  = 19
DATATYPE_STRING                       = 20
DATATYPE_BUFFER128                    = 21

DATATYPES_LABEL = (DATATYPE_LOCAL_LABEL, DATATYPE_GLOBAL_LABEL)
DATATYPES_NUMERIC = (DATATYPE_INT8, DATATYPE_INT16, DATATYPE_INT32, DATATYPE_FLOAT)
DATATYPES_STRING  = (DATATYPE_TEXTLABEL8, DATATYPE_TEXTLABEL16, DATATYPE_STRING, DATATYPE_BUFFER128)
DATATYPES_GLOBALVARS = (DATATYPE_GLOBALVAR_NUMBER, DATATYPE_GLOBALVAR_TEXTLABEL, DATATYPE_GLOBALVAR_TEXTLABEL16)
DATATYPES_LOCALVARS  = (DATATYPE_LOCALVAR_NUMBER, DATATYPE_LOCALVAR_TEXTLABEL, DATATYPE_LOCALVAR_TEXTLABEL16)
DATATYPES_GLOBALVARS_ARRAY = (DATATYPE_GLOBALVAR_ARRAY_NUMBER, DATATYPE_GLOBALVAR_ARRAY_TEXTLABEL, DATATYPE_GLOBALVAR_ARRAY_TEXTLABEL16)
DATATYPES_LOCALVARS_ARRAY  = (DATATYPE_LOCALVAR_ARRAY_NUMBER, DATATYPE_LOCALVAR_ARRAY_TEXTLABEL, DATATYPE_LOCALVAR_ARRAY_TEXTLABEL16)
DATATYPES_GLOBAVARS_ALL = DATATYPES_GLOBALVARS + DATATYPES_GLOBALVARS_ARRAY
DATATYPES_LOCALVARS_ALL = DATATYPES_LOCALVARS + DATATYPES_LOCALVARS_ARRAY
DATATYPES_VARS_ALL = DATATYPES_GLOBAVARS_ALL + DATATYPES_LOCALVARS_ALL

ARRAY_ELEM_TYPE_INT = 0
ARRAY_ELEM_TYPE_FLOAT = 1
ARRAY_ELEM_TYPE_TEXTLABEL = 2
ARRAY_ELEM_TYPE_TEXTLABEL16 = 3
ARRAY_ELEM_TYPES = (ARRAY_ELEM_TYPE_INT, ARRAY_ELEM_TYPE_FLOAT, ARRAY_ELEM_TYPE_TEXTLABEL, ARRAY_ELEM_TYPE_TEXTLABEL16)

class Bytecode:
    def __init__(self, main_block, mission_blocks=[], streamed_blocks=[]):
        self.main_block = main_block
        self.mission_blocks = mission_blocks
        self.streamed_blocks = streamed_blocks

    def __str__(self):
        lines = []
        lines.extend(str(data) for data in self.main_block)
        for i, block in enumerate(self.mission_blocks):
            lines.append("#MISSION_BLOCK_START %d" % i)
            lines.extend(str(data) for data in block)
            lines.append("#MISSION_BLOCK_END")
        for i, block in enumerate(self.streamed_blocks):
            lines.append("#STREAMED_BLOCK_START %d" % i)
            lines.extend(str(data) for data in block)
            lines.append("#STREAMED_BLOCK_END")
        return "\n".join(lines)

class Data:
    def __init__(self, xtype):   # no need to call
        self.type = xtype

    def __str__(self):
        raise NotImplementedError # derived shall implement

class Arg:
    def __init__(self, datatype): # no need to call
        self.type = datatype

    def __str__(self):
        raise NotImplementedError # derived shall implement

    def is_hex(self):
        return self.type == DATA_HEX

    def is_label(self):
        return self.type == DATA_LABEL

    def is_command(self):
        return self.type == DATA_COMMAND

class Label(Data):
    def __init__(self, name):
        self.type = DATA_LABEL
        self.name = name

    def __str__(self):
        return "%s:" % self.name

class Hex(Data):
    def __init__(self, bytearray_object):
        self.type = DATA_HEX
        self.bytes = bytearray_object

    def __str__(self):
        output = "IR2_HEX"
        for i in self.bytes:
            output += ' '
            output += str(i - 256 if i > 127 else i) + 'i8'
        return output

class Command(Data):
    def __init__(self, not_flag, name, args):
        self.type = DATA_COMMAND
        self.not_flag = not_flag
        self.name = name
        self.args = args

    def __str__(self):
        output = "NOT " if self.not_flag else ""
        output += self.name
        for a in self.args:
            output += ' '
            output += str(a)
        return output

class ArgNumber(Arg):
    def __init__(self, numtype, value):
        assert numtype in DATATYPES_NUMERIC
        self.type = numtype
        self.value = value

    def __str__(self):
        if self.type == DATATYPE_INT8:
            return "%di8" % self.value
        if self.type == DATATYPE_INT16:
            return "%di16" % self.value
        if self.type == DATATYPE_INT32:
            return "%di32" % self.value
        if self.type == DATATYPE_FLOAT:
            output = "%f" % self.value
            output = output.rstrip('0')
            if output[-1] == '.': output += '0'
            return output + 'f'
        assert False

class ArgLabel(Arg):
    def __init__(self, labtype, value):
        assert labtype in DATATYPES_LABEL
        self.type = labtype
        self.value = value

    def __str__(self):
        if self.type == DATATYPE_GLOBAL_LABEL:
            return "@%s" % self.value
        if self.type == DATATYPE_LOCAL_LABEL:
            return "%%%s" % self.value
        assert False

class ArgString(Arg):
    def __init__(self, strtype, value):
        assert strtype in DATATYPES_STRING
        self.type = strtype
        self.value = value

    def __str__(self):
        # TODO unescape
        if self.type == DATATYPE_TEXTLABEL8:
            return "'%s'" % self.value
        if self.type == DATATYPE_TEXTLABEL16:
            return "v'%s'" % self.value
        if self.type == DATATYPE_STRING:
            return "\"%s\"" % self.value
        if self.type == DATATYPE_BUFFER128:
            return "b\"%s\"" % self.value
        assert False

class ArgVariable(Arg):
    def __init__(self, vartype, offset):
        assert vartype in DATATYPES_GLOBALVARS or vartype in DATATYPES_LOCALVARS
        self.type = vartype
        self.offset = offset # i*1 for globals, i*4 for locals

    def is_global(self):
        return self.type in DATATYPES_GLOBALVARS

    def is_local(self):
        return self.type in DATATYPES_LOCALVARS

    def __str__(self):
        # TODO unescape
        c = _char_from_vartype(self.type)
        if self.is_global():
            return c + ("&%s" % (self.offset))
        else:
            return ("%s@" % (self.offset / 4)) + c
        assert False

class ArgArray(Arg):
    def __init__(self, base, index, size, elem_type):
        assert elem_type in ARRAY_ELEM_TYPES
        self.type = _type_from_base(base)
        self.base = base
        self.index = index
        self.size = size
        self.elem_type = elem_type

    def __str__(self):
        etc = _char_from_elemtype(self.elem_type)
        return "%s(%s,%d%c)" % (self.base, self.index, self.size, etc)

    @staticmethod
    def _type_from_base(base):
        if base.type in DATATYPES_GLOBALVARS:
            return DATATYPES_GLOBALVARS_ARRAY[DATATYPES_GLOBALVARS.index(base.type)]
        elif base.type in DATATYPES_LOCALVARS:
            return DATATYPES_LOCALVARS_ARRAY[DATATYPES_LOCALVARS.index(base.type)]
        else:
            raise ValueError("base is not a variable")


def read_ir2(file):
    try:
        lines = file.readlines()
    except AttributeError:
        with open(file) as f:
            return read_ir2(f)

    RE_INT8 = re.compile(r"^(-?[0-9]+)i8$")
    RE_INT16 = re.compile(r"^(-?[0-9]+)i16$")
    RE_INT32 = re.compile(r"^(-?[0-9]+)i32$")
    RE_FLOAT = re.compile(r"^(-?[0-9]+\.[0-9]+(?:e-?[0-9]+)?)f$")
    RE_GLOBALOFF = re.compile(r"^@([_A-Z][_A-Z0-9]*)$")
    RE_LOCALOFF = re.compile(r"^%([_A-Z][_A-Z0-9]*)$")
    RE_GLOBALVAR = re.compile(r"^([sv]?)&([0-9]+)$")
    RE_LOCALVAR = re.compile(r"^([0-9]+)@([sv]?)$")
    RE_ARRAY = re.compile(r"^([sv&@0-9]+)\(([&@0-9]+),([0-9]+)([ifsv])\)$")
    RE_TEXTLABEL = re.compile(r"^'([\x20-\x7E]*)'$")
    RE_TEXTLABEL16 = re.compile(r"^v'([\x20-\x7E]*)'$")
    RE_BUFFER128 = re.compile(r"^b\"([\x20-\x7E]*)\"$")
    RE_STRING = re.compile(r"^\"([\x20-\x7E]*)\"$")

    def escape(string):
        return string # TODO

    def var_datatype_from_char(c, tup):
        if c == '': return tup[0]
        if c == 's': return tup[1]
        if c == 'v': return tup[2]
        return None

    def var_from_token(token):
        m = RE_GLOBALVAR.match(token)
        if m != None:
            datatype = var_datatype_from_char(m.group(1), DATATYPES_GLOBALVARS)
            return ArgVariable(datatype, int(m.group(2)))

        m = RE_LOCALVAR.match(token)
        if m != None:
            datatype = var_datatype_from_char(m.group(2), DATATYPES_LOCALVARS)
            return ArgVariable(datatype, 4 * int(m.group(1)))

        return None

    def elem_from_token(token):
        if token == 'i': return ARRAY_ELEM_TYPE_INT
        if token == 'f': return ARRAY_ELEM_TYPE_FLOAT
        if token == 's': return ARRAY_ELEM_TYPE_TEXTLABEL
        if token == 's': return ARRAY_ELEM_TYPE_TEXTLABEL16
        return None

    def arg_from_token(token):
        m = RE_INT8.match(token)
        if m != None: return ArgNumber(DATATYPE_INT8, int(m.group(1)))

        m = RE_INT16.match(token)
        if m != None: return ArgNumber(DATATYPE_INT16, int(m.group(1)))
            
        m = RE_INT32.match(token)
        if m != None: return ArgNumber(DATATYPE_INT32, int(m.group(1)))

        m = RE_FLOAT.match(token)
        if m != None: return ArgNumber(DATATYPE_FLOAT, float(m.group(1)))

        m = RE_GLOBALOFF.match(token)
        if m != None: return ArgLabel(DATATYPE_GLOBAL_LABEL, m.group(1))

        m = RE_LOCALOFF.match(token)
        if m != None: return ArgLabel(DATATYPE_LOCAL_LABEL, m.group(1))

        a = var_from_token(token)
        if a != None: return a

        m = RE_ARRAY.match(token)
        if m != None:
            base = var_from_token(RE_ARRAY.group(1))
            index = var_from_token(RE_ARRAY.group(2))
            size = int(RE_ARRAY.group(3))
            elem = elem_from_token(RE_ARRAY.group(4))
            return ArgArray(base, index, size, elem)

        m = RE_TEXTLABEL.match(token)
        if m != None: return ArgString(DATATYPE_TEXTLABEL8, escape(m.group(1)))

        m = RE_TEXTLABEL16.match(token)
        if m != None: return ArgString(DATATYPE_TEXTLABEL16, escape(m.group(1)))        

        m = RE_BUFFER128.match(token)
        if m != None: return ArgString(DATATYPE_BUFFER128, escape(m.group(1)))

        m = RE_STRING.match(token)
        if m != None: return ArgString(DATATYPE_STRING, escape(m.group(1)))

        print(token)
        assert False

    main_block = []
    mission_blocks = []
    streamed_blocks = []

    current_block = main_block

    for line in lines:
        line = line.rstrip('\r\n')
        assert len(line) > 0 and not line[0].isspace() and not line[-1].isspace()
        if line[0] == '#':
            if line.startswith("#MISSION_BLOCK_START"):
                assert len(mission_blocks) == int(line.split()[1])
                current_block = []
            elif line.startswith("#STREAMED_BLOCK_START"):
                assert len(streamed_blocks) == int(line.split()[1])
                current_block = []
            elif line.startswith("#MISSION_BLOCK_END"):
                mission_blocks.append(current_block)
                current_block = None
            elif line.startswith("#STREAMED_BLOCK_END"):
                streamed_blocks.append(current_block)
                current_block = None
        elif line[-1] == ':':
            label = Label(line[:-1])
            current_block.append(label)
        else:
            tokens = [p for p in re.split("( |b?\\\".*?\\\"|v?'.*?')", line) if p.strip()]
            not_flag = (tokens[0] == "NOT")
            cmdname  = tokens[not_flag].upper()
            cmdargs = [arg_from_token(tokens[i]) for i in range(1 + not_flag, len(tokens))]
            if cmdname == "IR2_HEX":
                bytedata = bytearray([(i + 256 if i < 0 else i) for i in map(lambda a: a.value, cmdargs)])
                current_block.append(Hex(bytedata))
            else:
                current_block.append(Command(not_flag, cmdname, cmdargs))

    return Bytecode(main_block, mission_blocks, streamed_blocks)


def _char_from_vartype(vartype):
    assert vartype in DATATYPES_GLOBALVARS or vartype in DATATYPES_LOCALVARS
    if vartype in DATATYPES_LOCALVARS:
        return _char_from_vartype(DATATYPES_GLOBALVARS[DATATYPES_LOCALVARS.index(vartype)])
    if vartype == DATATYPE_GLOBALVAR_NUMBER:
        return ''
    if vartype == DATATYPE_GLOBALVAR_TEXTLABEL:
        return 's'
    if vartype == DATATYPE_GLOBALVAR_TEXTLABEL16:
        return 's'
    assert False

def _char_from_elemtype(elem):
    assert elemtype in ARRAY_ELEM_TYPES
    if elem == ARRAY_ELEM_TYPE_INT: return 'i'
    if elem == ARRAY_ELEM_TYPE_FLOAT: return 'f'
    if elem == ARRAY_ELEM_TYPE_TEXTLABEL: return 's'
    if elem == ARRAY_ELEM_TYPE_TEXTLABEL16: return 'v'
    assert False

if __name__ == "__main__":
    import sys
    ir2 = read_ir2(sys.argv[1])
    sys.stdout.write(str(ir2))
