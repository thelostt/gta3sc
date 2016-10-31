// RUN: %gta3sc %s --config=gtasa --guesser -emit-ir2 | %FileCheck %s

{
    VAR_INT  g
    LVAR_INT t

    VAR_INT  gi[2]
    LVAR_INT ti[2]

    VAR_FLOAT gf[2]
    LVAR_FLOAT tf[2]

    VAR_TEXT_LABEL gs[2]
    LVAR_TEXT_LABEL ts[2]

    VAR_TEXT_LABEL16 gv[2]
    LVAR_TEXT_LABEL16 tv[2]

    VAR_INT  g_max_offset
    LVAR_INT t_max_offset

    // CHECK-NEXT-L: SET &12 0i8
    gi[1] = 0
    // CHECK-NEXT-L: SET &16 1i8
    gi[2] = 1
    // CHECK-NEXT-L: SET &20 0.0f
    gf[1] = 0.0
    // CHECK-NEXT-L: SET &24 1.0f
    gf[2] = 1.0
    // CHECK-NEXT-L: SET s&28 'NAME1'
    gs[1] = NAME1
    // CHECK-NEXT-L: SET s&36 'NAME2'
    gs[2] = NAME2
    // CHECK-NEXT-L: SET v&44 "NAME3"
    gv[1] = NAME3
    // CHECK-NEXT-L: SET v&60 "NAME4"
    gv[2] = NAME4

    // CHECK-NEXT-L: SET 1@ 0i8
    ti[1] = 0
    // CHECK-NEXT-L: SET 2@ 1i8
    ti[2] = 1
    // CHECK-NEXT-L: SET 3@ 0.0f
    tf[1] = 0.0
    // CHECK-NEXT-L: SET 4@ 1.0f
    tf[2] = 1.0
    // CHECK-NEXT-L: SET 5@s 'NAME1' 
    ts[1] = NAME1
    // CHECK-NEXT-L: SET 7@s 'NAME2'
    ts[2] = NAME2
    // CHECK-NEXT-L: SET 9@v "NAME3"
    tv[1] = NAME3
    // CHECK-NEXT-L: SET 13@v "NAME4"
    tv[2] = NAME4

    // CHECK-NEXT-L: SET &12(&8,2i) 1i8
    gi[g] = 1
    // CHECK-NEXT-L: SET &12(0@,2i) 2i8 
    gi[t] = 2
    // CHECK-NEXT-L: SET &20(&8,2f) 1.0f 
    gf[g] = 1.0
    // CHECK-NEXT-L: SET &20(0@,2f) 2.0f
    gf[t] = 2.0
    // CHECK-NEXT-L: SET s&28(&8,2s) 'NAME1' 
    gs[g] = NAME1
    // CHECK-NEXT-L: SET s&28(0@,2s) 'NAME2' 
    gs[t] = NAME2
    // CHECK-NEXT-L: SET v&44(&8,2v) "NAME3"
    gv[g] = NAME3
    // CHECK-NEXT-L: SET v&44(0@,2v) "NAME4"
    gv[t] = NAME4

    // CHECK-NEXT-L: SET 1@(&8,2i) 1i8
    ti[g] = 1
    // CHECK-NEXT-L: SET 1@(0@,2i) 2i8
    ti[t] = 2
    // CHECK-NEXT-L: SET 3@(&8,2f) 1.0f
    tf[g] = 1.0
    // CHECK-NEXT-L: SET 3@(0@,2f) 2.0f
    tf[t] = 2.0
    // CHECK-NEXT-L: SET 5@s(&8,2s) 'NAME1'
    ts[g] = NAME1
    // CHECK-NEXT-L: SET 5@s(0@,2s) 'NAME2'
    ts[t] = NAME2
    // CHECK-NEXT-L: SET 9@v(&8,2v) "NAME3"
    tv[g] = NAME3
    // CHECK-NEXT-L: SET 9@v(0@,2v) "NAME4"
    tv[t] = NAME4

    // CHECK-NEXT-L: SET &76 0i8
    g_max_offset = 0
    // CHECK-NEXT-L: SET 17@ 0i8
    t_max_offset = 0
}

TERMINATE_THIS_SCRIPT