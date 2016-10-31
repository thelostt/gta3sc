// RUN: %gta3sc %s --config=gta3 -emit-ir2 | %FileCheck %s

{
    VAR_INT  a b c
    LVAR_INT x y z

    // CHECK-NEXT-L: SET 0@ 2@
    x = z
    // CHECK-NEXT-L: SET &8 &16
    a = c
}

{
    VAR_INT  d e f
    LVAR_INT i j k

    // CHECK-NEXT-L: SET 0@ 2@
    i = k
    // CHECK-NEXT-L: SET &20 &28
    d = f
}

TERMINATE_THIS_SCRIPT