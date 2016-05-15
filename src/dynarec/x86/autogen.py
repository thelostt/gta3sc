#!/usr/bin/env python
import sys, os, subprocess

def main():
    dasc_files = [x for x in os.listdir(".") if x.endswith(".dasc")]
    for dasc_file in dasc_files:
        cxx_file = os.path.splitext(dasc_file)[0]
        try:
            minilua_path = "../../../deps/minilua.exe"
            dynasm_path  = "../../../deps/dynasm/dynasm.lua"
            output = subprocess.check_output([minilua_path, dynasm_path, "-o", cxx_file, dasc_file], stderr=subprocess.STDOUT)  
        except subprocess.CalledProcessError as e:
            print(e.output)
            print("\nFailed to generate DynASM output file %s." % (cxx_file))
            sys.exit(1)
        else:
            if len(output) > 0: print(output)
            print("Successfully generated DynASM output file %s." % (cxx_file))

if __name__ == "__main__":
    main()
