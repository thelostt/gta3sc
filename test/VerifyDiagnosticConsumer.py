#!/usr/bin/env python2
"""
    A standalone replication of Clang's verify diagnostics option (-verify).

    Implementation is very dummy (e.g. doesn't care about multiline comments), but works.

    Usage:
        cc test.c 2>&1 | py VerifyDiagnosticConsumer.py test.c

    For documentation:
        http://clang.llvm.org/doxygen/classclang_1_1VerifyDiagnosticConsumer.html#details

    MIT License:

        Copyright (c) 2016 Denilson das Merces Amorim

        Permission is hereby granted, free of charge, to any person obtaining a copy of this
        software and associated documentation files (the "Software"), to deal in the Software
        without restriction, including without limitation the rights to use, copy, modify, merge,
        publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
        to whom the Software is furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be included in all copies or
        substantial portions of the Software.

        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
        INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
        PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
        FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
        OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
        DEALINGS IN THE SOFTWARE.
"""
import sys
import re
import os

EXPECTED_FATALERROR = 0
EXPECTED_ERROR      = 1
EXPECTED_WARNING    = 2

RE_COMMENT1 = re.compile("^expected-((?:error)|(?:warning))(-re)?()(@[+-]?\d+)? {{(.*)}}") # () to align with...
RE_COMMENT2 = re.compile("^expected-((?:error)|(?:warning))(-re)?@([^:]+)(:\d+) {{(.*)}}") # ...groups here
RE_CCERROR = re.compile("^((?:\w:[\\/])?[^:]+):(\d+:)?(\d+:)?( (?:(?:error)|(?:warning)):)? (.*)$")

class DiagInfo:
    def __init__(self, location, lineno, diagtype, diagtext, rawtext=None):
        self.location = os.path.normpath(location).split(os.sep)
        self.lineno   = lineno       # may be None for cc output
        self.diagtype = diagtype     # may be None for cc output
        self.diagtext = diagtext
        self.detected = False        # mutable
        self.rawtext  = rawtext

    def matches(self, ccdiag):

        if len(self.location) > len(ccdiag.location):
            return False
        for component in zip(reversed(self.location), reversed(ccdiag.location)):
            if component[0] != component[1]:
                return False

        if (self.lineno   == ccdiag.lineno or self.lineno == 0) and\
            self.diagtype == ccdiag.diagtype:
                if isinstance(self.diagtext, basestring):
                    return self.diagtext in ccdiag.diagtext
                else: # regex
                    return self.diagtext.search(ccdiag.diagtext) != None

                return True
        return False

    def __str__(self):
        return self.rawtext

def parse_diags_in_source(lines, sourcename):
    output = []
    found_nodiag = False
    for lineno in range(1, 1+len(lines)):
        line = lines[lineno-1]
        
        comment_pos = line.find("//")
        if comment_pos == -1:
            continue
        
        comment = line[comment_pos+2:].strip()
        match = RE_COMMENT1.match(comment)
        if match is None:
            match = RE_COMMENT2.match(comment)
        if match is None:
            if comment == "expected-no-diagnostics":
                found_nodiag = True
            continue
            
        match = match.groups()

        diagtype = {
            "error": EXPECTED_ERROR,
            "warning": EXPECTED_WARNING,
        }.get(match[0])

        diagtext = re.compile(match[4]) if match[1] else match[4]

        if match[3]:
            xlineno = match[3][1:]
            if xlineno.startswith("+") or xlineno.startswith("-"):
                lineno += int(xlineno)
            else:
                lineno = int(xlineno)

        if match[2]:
            diagsource = match[2]
        else:
            diagsource = sourcename

        output.append(DiagInfo(diagsource, lineno, diagtype, diagtext, rawtext=comment))

    if found_nodiag and len(output) > 0:
        sys.stderr.write("verify: error: given 'expected-no-diagnostics' but diagnostics found on source file.\n")
        sys.exit(1)
    elif not found_nodiag and len(output) == 0:
        sys.stderr.write("verify: error: no diagnostics found on source file, but 'expected-no-diagnostics' not specified.\n")
        sys.exit(1)

    return output

def parse_diags_in_ccout(lines):
    output = []
    for line in lines:
        match = RE_CCERROR.match(line)
        if match is None:
            continue

        match = match.groups()

        location = match[0]
        lineno   = int(match[1][:-1]) if match[1] else None
        diagtype = {
            " error:": EXPECTED_ERROR,
            " warning:": EXPECTED_WARNING,
        }.get(match[3])
        diagtext = match[4]

        if not match[3]:
            continue

        output.append(DiagInfo(location, lineno, diagtype, diagtext, rawtext=line.strip()))
    
    return output

def main(ccout, sourcefile, sourcename):
    num_issues = 0

    ccdiags_lines = ccout.readlines()

    if True:
        for line in ccdiags_lines:
            sys.stderr.write(line)
    
    srcdiags = parse_diags_in_source(sourcefile.readlines(), sourcename)
    ccdiags  = parse_diags_in_ccout(ccdiags_lines)
    
    for ccdiag in ccdiags:
        for srcdiag in srcdiags:
            if srcdiag.matches(ccdiag):
                srcdiag.detected = True
                ccdiag.detected = True
                break
        else:
            num_issues = num_issues + 1
            sys.stderr.write("verify: error: unexpected compiler diagnostic: %s\n" % (ccdiag))

    for srcdiag in srcdiags:
        if not srcdiag.detected:
            num_issues = num_issues + 1
            sys.stderr.write("verify: error: expected compiler diagnostic not given by compiler: %s\n" % (srcdiag))

    if num_issues > 0:
        sys.exit(1)

if __name__ == "__main__":

    # Hack for Python3
    try:
        basestring
    except NameError:
        basestring = str

    with open(sys.argv[1]) as sourcefile:
        main(ccout=sys.stdin, sourcefile=sourcefile, sourcename=sys.argv[1])
