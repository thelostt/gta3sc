#!/usr/bin/env python2

from gta3sc import *

# WARNING TEXT_LABEL AllowGlobal AllowLocal = false
# TODO ADD ENUM TO LOAD_MISSION_AUDIO
# FIND NOPS and remove arguments from them and do supported=false
# add text limits

def main():
    gtasa_commands = {c.id: c for c in commands_from_xml("gtasa/commands.xml")}
    gtavc_commands = {c.id: c for c in commands_from_xml("gtavc/commands.xml")}
    gta3_commands  = {c.id: c for c in commands_from_xml("gta3/commands.xml")}

    for cmdid, sacmd in gtasa_commands.iteritems():
        vccmd = gtavc_commands.get(cmdid)
        g3cmd = gtavc_commands.get(cmdid)
        if vccmd is not None and sacmd.same_behaviour(vccmd):
            pass
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
                pass


if __name__ == "__main__":
    main()


