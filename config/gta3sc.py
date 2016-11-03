# -*- Python -*-

from lxml import objectify
from lxml import etree

__all__ = ['Config', 'Alternator', 'Enum', 'Command', 'Argument']

class Alternator:
    def __init__(self):
        self.name = ""
        self.alters = []

    @staticmethod
    def from_node(node):
        init = Alternator()
        init.name   = node.get("Name")
        init.alters = [a.get("Name") for a in node.iter("Alternative")]
        return init

    def to_node(self):
        node = etree.Element("Alternator", Name=self.name)
        for a in self.alters:
            etree.SubElement(node, "Alternative", Name=a)
        return node

class Enum:
    def __init__(self):
        self.name = ""
        self.is_global = False  # global is a python keyword
        self.constants = {}

    @staticmethod
    def from_node(node):
        init = Enum()
        init.name       = node.get("Name")
        init.is_global  = str2bool(node.get("Global", "false"))
        init.constants  = {}
        last_value = -1
        for a in node.iter("Constant"):
            maybe_value = a.get("Value")
            last_value = int(maybe_value, 0) if maybe_value is not None else last_value + 1
            init.constants[a.get("Name")] = last_value
        return init

    def to_node(self):
        last_value = -1
        node = etree.Element("Enum", Name=self.name)
        if self.is_global:
            node.set("Global", bool2str(self.is_global))
        for k,v in sorted(self.constants.items(), key=lambda x: x[1]):
            if v == last_value + 1:
                etree.SubElement(node, "Constant", Name=k)
            else:
                etree.SubElement(node, "Constant", Name=k, Value=str(v))
            last_value = v
        return node

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

    def same_behaviour(self, other):
        if self.id == other.id and len(self.args) == len(other.args):
            return all(a.same_behaviour(b) for a,b in zip(self.args, other.args))
        return False

    def has_optional(self):
        return len(self.args) > 0 and self.args[-1].optional == True

    @staticmethod
    def from_node(node):
        init = Command()
        init.name = node.get("Name")
        init.id = int(node.get("ID"), 16)
        init.supported = str2bool(node.get("Supported", "true"))
        init.args = []
        node_args = node.find("Args")
        if node_args is not None:
            for a in node_args.iter("Arg"):
                init.args.append(Argument.from_node(a))
        return init

    def to_node(self):
        node = etree.Element("Command", Name=self.name, ID=hex(self.id))
        if self.supported == False:
            node.set("Supported", bool2str(self.supported))
        if len(self.args) > 0:
            node_args = etree.SubElement(node, "Args")
            for a in self.args:
                node_args.append(a.to_node())
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
        self.enums = []

    def __eq__(self, other):
        return self.__dict__ == other.__dict__

    def same_behaviour(self, other):
        return self.type == other.type and\
               self.out == other.out and\
               self.ref == other.ref and\
               self.optional == other.optional

    def has_enum(self, name):
        return any(x == name for x in self.enums)

    @staticmethod
    def from_node(node):
        init = Argument()
        init.type = node.get("Type")
        init.desc = node.get("Desc", "")
        init.out  = str2bool(node.get("Out", "false"))
        init.ref  = str2bool(node.get("Ref", "false"))
        init.optional = str2bool(node.get("Optional", "false"))
        init.allow_const = str2bool(node.get("AllowConst", "true"))
        init.allow_gvar = str2bool(node.get("AllowGlobalVar", "true"))
        init.allow_lvar = str2bool(node.get("AllowLocalVar", "true"))
        init.entity = node.get("Entity", None)
        init.enums = node.get("Enum", None)
        init.enums = [init.enums] if init.enums else []
        return init
        
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
    
class Config:
    def __init__(self):
        self.commands = []
        self.enums = []
        self.alternators = []

    @staticmethod
    def from_file(filename):
        c = Config()
        c.read_config(filename)
        return c

    def read_config(self, filename):
        tree = etree.parse(filename)
        for item in tree.getroot():
            if item.tag == "Alternators":
                for subitem in item:
                    if subitem.tag == "Alternator":
                        self.alternators.append(Alternator.from_node(subitem))
            elif item.tag == "Commands":
                for subitem in item:
                    if subitem.tag == "Command":
                        self.commands.append(Command.from_node(subitem))
            elif item.tag == "Constants":
                for subitem in item:
                    if subitem.tag == "Enum":
                        self.enums.append(Enum.from_node(subitem))

    def save_config(self, filename, pretty_print=True):
        root = etree.Element("GTA3Script")
        if len(self.enums) > 0:
            base = etree.SubElement(root, "Constants") 
            for c in self.enums:
                base.append(c.to_node())
        if len(self.alternators) > 0:
            base = etree.SubElement(root, "Alternators") 
            for c in self.alternators:
                base.append(c.to_node())
        if len(self.commands) > 0:
            base = etree.SubElement(root, "Commands") 
            for c in self.commands:
                base.append(c.to_node())

        tree = etree.ElementTree(root)
        tree.write(filename, encoding="utf-8", pretty_print=pretty_print, xml_declaration=True)





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


if __name__ == "__main__":
    import sys
    cfg = Config()
    cfg.read_config(sys.argv[1])
    cfg.save_config(sys.argv[1])
