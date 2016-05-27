#!/usr/bin/env python2
"""
  Examples:
    py simplify.py out/gtasa/commands.xml out.xml
    py simplify.py out/gtasa/commands.xml out.xml any_3rd_arg_will_trigger_clear_useless
"""
from gta3sc import *
import sys

def main(xmlfile, outname, clear_useless_data):
    commands = commands_from_xml(xmlfile)

    if clear_useless_data:
        new_commands = []
        for cmd in commands:
            if cmd.supported == False:
                continue
            for a in cmd.args:
                a.desc   = ""
                a.entity = None
                a.enums  = []
                a.allow_const = True
                a.allow_gvar = True
                a.allow_lvar = True
            new_commands.append(cmd)
    else:
        # Simply rewriting the XML will simplify it.
        new_commands = commands

    commands_to_xml(outname, new_commands, pretty_print=(not clear_useless_data))

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: simplify.py <xmlfile> <outfile> <[clear_useless_data]>")
    main(sys.argv[1], sys.argv[2], len(sys.argv) > 3)
