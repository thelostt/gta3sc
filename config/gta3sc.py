from lxml import objectify
from lxml import etree

def is_arithmetic(op):
    return (op >= 0x4 and op <= 0x4B) or (op >= 0x58 and op <= 0x97) or\
           (op >= 0x4a3 and op <= 0x4a4) or (op >= 0x4ae and op <= 0x4b7) or\
           (op >= 1457 and op <= 1467)

class Command:
    def __init__(self):
        self.name = ""
        self.id = 0
        self.supported = False
        self.args = []

    def __eq__(self, other):
        return self.name == other.name and\
               self.id == other.id and\
               self.supported == other.supported and\
               self.args == other.args


    def has_optional(self):
        return len(self.args) > 0 and self.args[-1].optional == True

    def init_from_node(self, c):
        self.name      = c.get("Name")
        self.id        = int(c.get("ID"), 16)
        self.supported = str2bool(c.get("Supported", "true"))
        self.args      = []
        if hasattr(c, "Args") and hasattr(c.Args[0], "Arg"):
            for a in c.Args[0].Arg:
                arg = Argument()
                arg.init_from_node(a)
                self.args.append(arg)

    def to_node(self):
        node = etree.Element("Command", Name=self.name, ID=hex(self.id))
        if self.supported == False:
            node.set("Supported", bool2str(self.supported))
        if len(self.args) > 0:
            node_args = etree.Element("Args")
            for a in self.args:
                node_args.append(a.to_node())
            node.append(node_args)
        return node

class Argument:
    def __init__(self):
        self.type = "ANY"
        self.desc = ""
        self.out = False
        self.ref = False
        self.optional = False
        self.allow_const = False
        self.allow_gvar = False
        self.allow_lvar = False
        self.entity = None
        self.enums = None

    def __eq__(self, other):
        return self.__dict__ == other.__dict__

    def init_from_node(self, a):
        self.type = a.get("Type")
        self.desc = a.get("Desc", "")
        self.out  = str2bool(a.get("Out", "false"))
        self.ref  = str2bool(a.get("Ref", "false"))
        self.optional = str2bool(a.get("Optional", "false"))
        self.allow_const = str2bool(a.get("AllowConst", "true"))
        self.allow_gvar = str2bool(a.get("AllowGlobalVar", "true"))
        self.allow_lvar = str2bool(a.get("AllowLocalVar", "true"))
        self.entity = a.get("Entity", None)
        self.enums = a.get("Enum", None)
        self.enums = [self.enums] if self.enums else []
        
    def to_node(self):
        node = etree.Element("Arg", Type=self.type)
        if self.desc.strip() != "":
            node.set("Desc", self.desc)
        if self.out != False:
            node.set("Out", bool2str(self.out))
        if self.ref != False:
            node.set("Ref", bool2str(self.ref))
        if self.optional != False:
            node.set("Optional", bool2str(self.optional))
        if self.allow_const != True:
            node.set("AllowConst", bool2str(self.allow_const))
        if self.allow_gvar != True:
            node.set("AllowGlobalVar", bool2str(self.allow_gvar))
        if self.allow_lvar != True:
            node.set("AllowLocalVar", bool2str(self.allow_lvar))
        if self.entity != None:
            node.set("Entity", self.entity)
        if len(self.enums) != 0:
            assert len(self.enums) == 1
            node.set("Enum", self.enums[0])
        return node
    

def commands_from_xml(filename):
    commands = []
    with open(filename) as f:
        lines = "".join(f.readlines())
        xobjc = objectify.fromstring(lines)
        for node in xobjc.Commands[0].Command:
            c = Command()
            c.init_from_node(node)
            commands.append(c)
        return commands

def commands_to_xml(filename, commands, pretty_print=True):
    root = etree.Element("GTA3Script")
    base = etree.SubElement(root, "Commands") 
    for c in commands:
        base.append(c.to_node())

    with open(filename, 'w') as f:
        f.write(etree.tostring(root,
            pretty_print=pretty_print,
            encoding="utf-8",
            xml_declaration=True))



#
# Internal
#

def str2bool(x):
    if x == "true":
        return True
    if x == "false":
        return False
    print(x)
    assert False

def bool2str(x):
    if x == True:
        return "true"
    if x == False:
        return "false"
    print(type(x))
    assert False

