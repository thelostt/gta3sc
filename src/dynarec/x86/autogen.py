#!/usr/bin/env python
import sys, os, subprocess

def main():
    try:
        minilua_path = "../../../deps/minilua.exe"
        dynasm_path  = "../../../deps/dynasm/dynasm.lua"
        output = subprocess.check_output([minilua_path, dynasm_path, "-o", "test_autogen.cpp", "test.dasc"], stderr=subprocess.STDOUT)  
    except subprocess.CalledProcessError as e:
        print(e.output)
        print("\nFailed to generate DynASM output file.")
        sys.exit(1)
    else:
        if len(output) > 0: print(output)
        print("Successfully generated DynASM output file.")

if __name__ == "__main__":
    main()
