#include "defs.hpp"

Commands gtavc_commands()
{
    auto fade_enum = make_enum
    ({
        { "FADE_OUT", 0 },
        { "FADE_IN",  1 },
    });

    return Commands
    {
        // Commands
        {
            {
                "NOP",
                {
                    true,
                    0,
                    {
                    }
                }
            },
            {
                "WAIT",
                {
                    true,
                    1,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GOTO",
                {
                    true,
                    2,
                    {
                        { ArgType::Label, false, true, false, false,{} },
                    }
                }
            },
            {
                "SHAKE_CAM",
                {
                    true,
                    3,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET",
                {
                    true,
                    4,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET",
                {
                    true,
                    5,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET",
                {
                    true,
                    6,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET",
                {
                    true,
                    7,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING",
                {
                    true,
                    8,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING",
                {
                    true,
                    9,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING",
                {
                    true,
                    10,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING",
                {
                    true,
                    11,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING",
                {
                    true,
                    12,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING",
                {
                    true,
                    13,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING",
                {
                    true,
                    14,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING",
                {
                    true,
                    15,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "MULT_THING_BY_THING",
                {
                    true,
                    16,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "MULT_THING_BY_THING",
                {
                    true,
                    17,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "MULT_THING_BY_THING",
                {
                    true,
                    18,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "MULT_THING_BY_THING",
                {
                    true,
                    19,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "DIV_THING_BY_THING",
                {
                    true,
                    20,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "DIV_THING_BY_THING",
                {
                    true,
                    21,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "DIV_THING_BY_THING",
                {
                    true,
                    22,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "DIV_THING_BY_THING",
                {
                    true,
                    23,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    24,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    25,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    26,
                    {
                        { ArgType::Integer, false, true, false, false,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    27,
                    {
                        { ArgType::Integer, false, true, false, false,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    28,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    29,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    30,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    31,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    32,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    33,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    34,
                    {
                        { ArgType::Float, false, true, false, false,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    35,
                    {
                        { ArgType::Float, false, true, false, false,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    36,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    37,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    38,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    39,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    40,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    41,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    42,
                    {
                        { ArgType::Integer, false, true, false, false,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    43,
                    {
                        { ArgType::Integer, false, true, false, false,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    44,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    45,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    46,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    47,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    48,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    49,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    50,
                    {
                        { ArgType::Float, false, true, false, false,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    51,
                    {
                        { ArgType::Float, false, true, false, false,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    52,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    53,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    54,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    55,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_EQUAL_TO_THING",
                {
                    true,
                    56,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_EQUAL_TO_THING",
                {
                    true,
                    57,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_EQUAL_TO_THING",
                {
                    true,
                    58,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_EQUAL_TO_THING",
                {
                    true,
                    59,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_EQUAL_TO_THING",
                {
                    true,
                    60,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_NOT_EQUAL_TO_THING",
                {
                    true,
                    61,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_NOT_EQUAL_TO_THING",
                {
                    true,
                    62,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_NOT_EQUAL_TO_THING",
                {
                    true,
                    63,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_NOT_EQUAL_TO_THING",
                {
                    true,
                    64,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_NOT_EQUAL_TO_THING",
                {
                    true,
                    65,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_EQUAL_TO_THING",
                {
                    true,
                    66,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_EQUAL_TO_THING",
                {
                    true,
                    67,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_EQUAL_TO_THING",
                {
                    true,
                    68,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_EQUAL_TO_THING",
                {
                    true,
                    69,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_EQUAL_TO_THING",
                {
                    true,
                    70,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_NOT_EQUAL_TO_THING",
                {
                    true,
                    71,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_NOT_EQUAL_TO_THING",
                {
                    true,
                    72,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_THING_NOT_EQUAL_TO_THING",
                {
                    true,
                    73,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_NOT_EQUAL_TO_THING",
                {
                    true,
                    74,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_NOT_EQUAL_TO_THING",
                {
                    true,
                    75,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "GOTO_IF_TRUE",
                {
                    false,
                    76,
                    {
                        { ArgType::Label, false, true, false, false,{} },
                    }
                }
            },
            {
                "GOTO_IF_FALSE",
                {
                    true,
                    77,
                    {
                        { ArgType::Label, false, true, false, false,{} },
                    }
                }
            },
            {
                "TERMINATE_THIS_SCRIPT",
                {
                    true,
                    78,
                    {
                    }
                }
            },
            {
                "START_NEW_SCRIPT",
                {
                    true,
                    79,
                    {
                        { ArgType::Label, false, true, false, false,{} },
                        { ArgType::Any, true, true, true, true,{} },
                        { ArgType::Any, true, true, true, true,{} },
                        { ArgType::Any, true, true, true, true,{} },
                        { ArgType::Any, true, true, true, true,{} },
                        { ArgType::Any, true, true, true, true,{} },
                        { ArgType::Any, true, true, true, true,{} },
                        { ArgType::Any, true, true, true, true,{} },
                        { ArgType::Any, true, true, true, true,{} },
                        { ArgType::Any, true, true, true, true,{} },
                        { ArgType::Any, true, true, true, true,{} },
                        { ArgType::Any, true, true, true, true,{} },
                        { ArgType::Any, true, true, true, true,{} },
                        { ArgType::Any, true, true, true, true,{} },
                        { ArgType::Any, true, true, true, true,{} },
                        { ArgType::Any, true, true, true, true,{} },
                        { ArgType::Any, true, true, true, true,{} },
                        { ArgType::Any, true, true, true, true,{} },
                    }
                }
            },
            {
                "GOSUB",
                {
                    true,
                    80,
                    {
                        { ArgType::Label, false, true, false, false,{} },
                    }
                }
            },
            {
                "RETURN",
                {
                    true,
                    81,
                    {
                    }
                }
            },
            {
                "LINE",
                {
                    true,
                    82,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "CREATE_PLAYER",
                {
                    true,
                    83,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_PLAYER_COORDINATES",
                {
                    true,
                    84,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_COORDINATES",
                {
                    true,
                    85,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_AREA_2D",
                {
                    true,
                    86,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_AREA_3D",
                {
                    true,
                    87,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING",
                {
                    true,
                    88,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING",
                {
                    true,
                    89,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING",
                {
                    true,
                    90,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING",
                {
                    true,
                    91,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING",
                {
                    true,
                    92,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING",
                {
                    true,
                    93,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING",
                {
                    true,
                    94,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING",
                {
                    true,
                    95,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING",
                {
                    true,
                    96,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING",
                {
                    true,
                    97,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING",
                {
                    true,
                    98,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING",
                {
                    true,
                    99,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING",
                {
                    true,
                    100,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING",
                {
                    true,
                    101,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING",
                {
                    true,
                    102,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING",
                {
                    true,
                    103,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "MULT_THING_BY_THING",
                {
                    true,
                    104,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "MULT_THING_BY_THING",
                {
                    true,
                    105,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "MULT_THING_BY_THING",
                {
                    true,
                    106,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "MULT_THING_BY_THING",
                {
                    true,
                    107,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "MULT_THING_BY_THING",
                {
                    true,
                    108,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "MULT_THING_BY_THING",
                {
                    true,
                    109,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "MULT_THING_BY_THING",
                {
                    true,
                    110,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "MULT_THING_BY_THING",
                {
                    true,
                    111,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "DIV_THING_BY_THING",
                {
                    true,
                    112,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "DIV_THING_BY_THING",
                {
                    true,
                    113,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "DIV_THING_BY_THING",
                {
                    true,
                    114,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "DIV_THING_BY_THING",
                {
                    true,
                    115,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "DIV_THING_BY_THING",
                {
                    true,
                    116,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "DIV_THING_BY_THING",
                {
                    true,
                    117,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "DIV_THING_BY_THING",
                {
                    true,
                    118,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "DIV_THING_BY_THING",
                {
                    true,
                    119,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING_TIMED",
                {
                    true,
                    120,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING_TIMED",
                {
                    true,
                    121,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING_TIMED",
                {
                    true,
                    122,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING_TIMED",
                {
                    true,
                    123,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING_TIMED",
                {
                    true,
                    124,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "ADD_THING_TO_THING_TIMED",
                {
                    true,
                    125,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING_TIMED",
                {
                    true,
                    126,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING_TIMED",
                {
                    true,
                    127,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, true, false, false,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING_TIMED",
                {
                    true,
                    128,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING_TIMED",
                {
                    true,
                    129,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING_TIMED",
                {
                    true,
                    130,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "SUB_THING_FROM_THING_TIMED",
                {
                    true,
                    131,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "SET",
                {
                    true,
                    132,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "SET",
                {
                    true,
                    133,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "SET",
                {
                    true,
                    134,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "SET",
                {
                    true,
                    135,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "SET",
                {
                    true,
                    136,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "SET",
                {
                    true,
                    137,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "SET",
                {
                    true,
                    138,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "SET",
                {
                    true,
                    139,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "CSET",
                {
                    true,
                    140,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "CSET",
                {
                    true,
                    141,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "CSET",
                {
                    true,
                    142,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "CSET",
                {
                    true,
                    143,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "CSET",
                {
                    true,
                    144,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "CSET",
                {
                    true,
                    145,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "CSET",
                {
                    true,
                    146,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "CSET",
                {
                    true,
                    147,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "ABS",
                {
                    true,
                    148,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "ABS",
                {
                    true,
                    149,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "ABS",
                {
                    true,
                    150,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "ABS",
                {
                    true,
                    151,
                    {
                        { ArgType::Float, false, false, false, true,{} },
                    }
                }
            },
            {
                "GENERATE_RANDOM_FLOAT",
                {
                    true,
                    152,
                    {
                        { ArgType::Float, false, false, true, false,{} },
                    }
                }
            },
            {
                "GENERATE_RANDOM_INT",
                {
                    true,
                    153,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "CREATE_CHAR",
                {
                    true,
                    154,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "DELETE_CHAR",
                {
                    true,
                    155,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CHAR_WANDER_DIR",
                {
                    true,
                    156,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CHAR_WANDER_RANGE",
                {
                    false,
                    157,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CHAR_FOLLOW_PATH",
                {
                    true,
                    158,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CHAR_SET_IDLE",
                {
                    true,
                    159,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_CHAR_COORDINATES",
                {
                    true,
                    160,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_COORDINATES",
                {
                    true,
                    161,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_STILL_ALIVE",
                {
                    false,
                    162,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_AREA_2D",
                {
                    true,
                    163,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_AREA_3D",
                {
                    true,
                    164,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CREATE_CAR",
                {
                    true,
                    165,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "DELETE_CAR",
                {
                    true,
                    166,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CAR_GOTO_COORDINATES",
                {
                    true,
                    167,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "CAR_WANDER_RANDOMLY",
                {
                    true,
                    168,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CAR_SET_IDLE",
                {
                    true,
                    169,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_CAR_COORDINATES",
                {
                    true,
                    170,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_COORDINATES",
                {
                    true,
                    171,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_STILL_ALIVE",
                {
                    false,
                    172,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_CRUISE_SPEED",
                {
                    true,
                    173,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_DRIVING_STYLE",
                {
                    true,
                    174,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_MISSION",
                {
                    true,
                    175,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_IN_AREA_2D",
                {
                    true,
                    176,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_IN_AREA_3D",
                {
                    true,
                    177,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_BIG",
                {
                    true,
                    186,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "PRINT",
                {
                    true,
                    187,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "PRINT_NOW",
                {
                    true,
                    188,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "PRINT_SOON",
                {
                    false,
                    189,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "CLEAR_PRINTS",
                {
                    true,
                    190,
                    {
                    }
                }
            },
            {
                "GET_TIME_OF_DAY",
                {
                    true,
                    191,
                    {
                        { ArgType::Integer, false, false, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_TIME_OF_DAY",
                {
                    true,
                    192,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_MINUTES_TO_TIME_OF_DAY",
                {
                    true,
                    193,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_POINT_ON_SCREEN",
                {
                    true,
                    194,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "DEBUG_ON",
                {
                    true,
                    195,
                    {
                    }
                }
            },
            {
                "DEBUG_OFF",
                {
                    true,
                    196,
                    {
                    }
                }
            },
            {
                "RETURN_TRUE",
                {
                    false,
                    197,
                    {
                    }
                }
            },
            {
                "RETURN_FALSE",
                {
                    false,
                    198,
                    {
                    }
                }
            },
            {
                "ANDOR",
                {
                    true,
                    214,
                    {
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "LAUNCH_MISSION",
                {
                    true,
                    215,
                    {
                        { ArgType::Label, false, true, false, false,{} },
                    }
                }
            },
            {
                "MISSION_HAS_FINISHED",
                {
                    true,
                    216,
                    {
                    }
                }
            },
            {
                "STORE_CAR_CHAR_IS_IN",
                {
                    true,
                    217,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "STORE_CAR_PLAYER_IS_IN",
                {
                    true,
                    218,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_CAR",
                {
                    true,
                    219,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_CAR",
                {
                    true,
                    220,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_MODEL",
                {
                    true,
                    221,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_MODEL",
                {
                    true,
                    222,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_ANY_CAR",
                {
                    true,
                    223,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_ANY_CAR",
                {
                    true,
                    224,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_BUTTON_PRESSED",
                {
                    true,
                    225,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_PAD_STATE",
                {
                    true,
                    226,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_ANY_MEANS_2D",
                {
                    true,
                    227,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_ON_FOOT_2D",
                {
                    true,
                    228,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_IN_CAR_2D",
                {
                    true,
                    229,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_PLAYER_ANY_MEANS_2D",
                {
                    true,
                    230,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_PLAYER_ON_FOOT_2D",
                {
                    true,
                    231,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_PLAYER_IN_CAR_2D",
                {
                    true,
                    232,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_ANY_MEANS_CHAR_2D",
                {
                    true,
                    233,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_ON_FOOT_CHAR_2D",
                {
                    true,
                    234,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_IN_CAR_CHAR_2D",
                {
                    true,
                    235,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_ANY_MEANS_2D",
                {
                    true,
                    236,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_ON_FOOT_2D",
                {
                    true,
                    237,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_IN_CAR_2D",
                {
                    true,
                    238,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_CHAR_ANY_MEANS_2D",
                {
                    true,
                    239,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_CHAR_ON_FOOT_2D",
                {
                    true,
                    240,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_CHAR_IN_CAR_2D",
                {
                    true,
                    241,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_ANY_MEANS_CHAR_2D",
                {
                    true,
                    242,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_ON_FOOT_CHAR_2D",
                {
                    true,
                    243,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_IN_CAR_CHAR_2D",
                {
                    true,
                    244,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_ANY_MEANS_3D",
                {
                    true,
                    245,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_ON_FOOT_3D",
                {
                    true,
                    246,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_IN_CAR_3D",
                {
                    true,
                    247,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_PLAYER_ANY_MEANS_3D",
                {
                    true,
                    248,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_PLAYER_ON_FOOT_3D",
                {
                    true,
                    249,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_PLAYER_IN_CAR_3D",
                {
                    true,
                    250,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_ANY_MEANS_CHAR_3D",
                {
                    true,
                    251,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_ON_FOOT_CHAR_3D",
                {
                    true,
                    252,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_IN_CAR_CHAR_3D",
                {
                    true,
                    253,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_ANY_MEANS_3D",
                {
                    true,
                    254,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_ON_FOOT_3D",
                {
                    true,
                    255,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_IN_CAR_3D",
                {
                    true,
                    256,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_CHAR_ANY_MEANS_3D",
                {
                    true,
                    257,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_CHAR_ON_FOOT_3D",
                {
                    true,
                    258,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_CHAR_IN_CAR_3D",
                {
                    true,
                    259,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_ANY_MEANS_CHAR_3D",
                {
                    true,
                    260,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_ON_FOOT_CHAR_3D",
                {
                    true,
                    261,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_IN_CAR_CHAR_3D",
                {
                    true,
                    262,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CREATE_OBJECT",
                {
                    true,
                    263,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "DELETE_OBJECT",
                {
                    true,
                    264,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_SCORE",
                {
                    true,
                    265,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_SCORE_GREATER",
                {
                    true,
                    266,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "STORE_SCORE",
                {
                    true,
                    267,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "GIVE_REMOTE_CONTROLLED_CAR_TO_PLAYER",
                {
                    true,
                    268,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "ALTER_WANTED_LEVEL",
                {
                    true,
                    269,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ALTER_WANTED_LEVEL_NO_DROP",
                {
                    true,
                    270,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_WANTED_LEVEL_GREATER",
                {
                    true,
                    271,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_WANTED_LEVEL",
                {
                    true,
                    272,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_DEATHARREST_STATE",
                {
                    true,
                    273,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_DEATHARREST_BEEN_EXECUTED",
                {
                    true,
                    274,
                    {
                    }
                }
            },
            {
                "ADD_AMMO_TO_PLAYER",
                {
                    false,
                    275,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_AMMO_TO_CHAR",
                {
                    true,
                    276,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_AMMO_TO_CAR",
                {
                    false,
                    277,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_STILL_ALIVE",
                {
                    false,
                    278,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_DEAD",
                {
                    true,
                    279,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_DEAD",
                {
                    true,
                    280,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_DEAD",
                {
                    true,
                    281,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_THREAT_SEARCH",
                {
                    true,
                    282,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_THREAT_REACTION",
                {
                    false,
                    283,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_NO_OBJ",
                {
                    true,
                    284,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ORDER_DRIVER_OUT_OF_CAR",
                {
                    false,
                    285,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ORDER_CHAR_TO_DRIVE_CAR",
                {
                    false,
                    286,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_PATROL_POINT",
                {
                    false,
                    287,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_GANGZONE",
                {
                    false,
                    288,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_ZONE",
                {
                    true,
                    289,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_PLAYER_PRESSING_HORN",
                {
                    true,
                    290,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_CHAR_SPOTTED_PLAYER",
                {
                    true,
                    291,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ORDER_CHAR_TO_BACKDOOR",
                {
                    false,
                    292,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_CHAR_TO_GANG",
                {
                    false,
                    293,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_OBJECTIVE_PASSED",
                {
                    true,
                    294,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_DRIVE_AGGRESSION",
                {
                    false,
                    295,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_MAX_DRIVESPEED",
                {
                    false,
                    296,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "CREATE_CHAR_INSIDE_CAR",
                {
                    true,
                    297,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "WARP_PLAYER_FROM_CAR_TO_COORD",
                {
                    true,
                    298,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "MAKE_CHAR_DO_NOTHING",
                {
                    false,
                    299,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_INVINCIBLE",
                {
                    false,
                    300,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_INVINCIBLE",
                {
                    false,
                    301,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_GRAPHIC_TYPE",
                {
                    false,
                    302,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_GRAPHIC_TYPE",
                {
                    false,
                    303,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_PLAYER_BEEN_ARRESTED",
                {
                    false,
                    304,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "STOP_CHAR_DRIVING",
                {
                    false,
                    305,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "KILL_CHAR",
                {
                    false,
                    306,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_FAVOURITE_CAR_MODEL_FOR_CHAR",
                {
                    false,
                    307,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OCCUPATION",
                {
                    false,
                    308,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CHANGE_CAR_LOCK",
                {
                    false,
                    309,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SHAKE_CAM_WITH_POINT",
                {
                    false,
                    310,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_MODEL",
                {
                    true,
                    311,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_REMAP",
                {
                    false,
                    312,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_CAR_JUST_SUNK",
                {
                    false,
                    313,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_NO_COLLIDE",
                {
                    false,
                    314,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_DEAD_IN_AREA_2D",
                {
                    false,
                    315,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_DEAD_IN_AREA_3D",
                {
                    false,
                    316,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_TRAILER_ATTACHED",
                {
                    false,
                    317,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_ON_TRAILER",
                {
                    false,
                    318,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_CAR_GOT_WEAPON",
                {
                    false,
                    319,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PARK",
                {
                    false,
                    320,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_PARK_FINISHED",
                {
                    false,
                    321,
                    {
                    }
                }
            },
            {
                "KILL_ALL_PASSENGERS",
                {
                    false,
                    322,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_BULLETPROOF",
                {
                    false,
                    323,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_FLAMEPROOF",
                {
                    false,
                    324,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_ROCKETPROOF",
                {
                    false,
                    325,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CARBOMB_ACTIVE",
                {
                    false,
                    326,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GIVE_CAR_ALARM",
                {
                    false,
                    327,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PUT_CAR_ON_TRAILER",
                {
                    false,
                    328,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_CRUSHED",
                {
                    false,
                    329,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CREATE_GANG_CAR",
                {
                    false,
                    330,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "CREATE_CAR_GENERATOR",
                {
                    true,
                    331,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_CAR_GENERATOR",
                {
                    true,
                    332,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_PAGER_MESSAGE",
                {
                    false,
                    333,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "DISPLAY_ONSCREEN_TIMER",
                {
                    true,
                    334,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_ONSCREEN_TIMER",
                {
                    true,
                    335,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "DISPLAY_ONSCREEN_COUNTER",
                {
                    false,
                    336,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_ONSCREEN_COUNTER",
                {
                    true,
                    337,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "SET_ZONE_CAR_INFO",
                {
                    true,
                    338,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_GANG_ZONE",
                {
                    false,
                    339,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_ZONE",
                {
                    true,
                    340,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_CAR_DENSITY",
                {
                    false,
                    341,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PED_DENSITY",
                {
                    false,
                    342,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "POINT_CAMERA_AT_PLAYER",
                {
                    true,
                    343,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "POINT_CAMERA_AT_CAR",
                {
                    true,
                    344,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "POINT_CAMERA_AT_CHAR",
                {
                    true,
                    345,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "RESTORE_CAMERA",
                {
                    true,
                    346,
                    {
                    }
                }
            },
            {
                "SHAKE_PAD",
                {
                    false,
                    347,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_ZONE_PED_INFO",
                {
                    true,
                    348,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TIME_SCALE",
                {
                    true,
                    349,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_IN_AIR",
                {
                    false,
                    350,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_FIXED_CAMERA_POSITION",
                {
                    true,
                    351,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "POINT_CAMERA_AT_POINT",
                {
                    true,
                    352,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_BLIP_FOR_CAR_OLD",
                {
                    true,
                    353,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_BLIP_FOR_CHAR_OLD",
                {
                    true,
                    354,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_BLIP_FOR_OBJECT_OLD",
                {
                    false,
                    355,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "REMOVE_BLIP",
                {
                    true,
                    356,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CHANGE_BLIP_COLOUR",
                {
                    true,
                    357,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DIM_BLIP",
                {
                    true,
                    358,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_BLIP_FOR_COORD_OLD",
                {
                    true,
                    359,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "CHANGE_BLIP_SCALE",
                {
                    true,
                    360,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_FADING_COLOUR",
                {
                    true,
                    361,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DO_FADE",
                {
                    true,
                    362,
                    {
                        { ArgType::Integer, false, true, true, true, {} },
                        { ArgType::Integer, false, true, true, true, { fade_enum } },
                    }
                }
            },
            {
                "GET_FADING_STATUS",
                {
                    true,
                    363,
                    {
                    }
                }
            },
            {
                "ADD_HOSPITAL_RESTART",
                {
                    true,
                    364,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_POLICE_RESTART",
                {
                    true,
                    365,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "OVERRIDE_NEXT_RESTART",
                {
                    true,
                    366,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "DRAW_SHADOW",
                {
                    false,
                    367,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_PLAYER_HEADING",
                {
                    true,
                    368,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_HEADING",
                {
                    true,
                    369,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_CHAR_HEADING",
                {
                    true,
                    370,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_HEADING",
                {
                    true,
                    371,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_CAR_HEADING",
                {
                    true,
                    372,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_HEADING",
                {
                    true,
                    373,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_OBJECT_HEADING",
                {
                    true,
                    374,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_OBJECT_HEADING",
                {
                    true,
                    375,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_TOUCHING_OBJECT",
                {
                    false,
                    376,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_TOUCHING_OBJECT",
                {
                    true,
                    377,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_AMMO",
                {
                    true,
                    378,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_AMMO",
                {
                    false,
                    379,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_AMMO",
                {
                    false,
                    380,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOAD_CAMERA_SPLINE",
                {
                    false,
                    381,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "MOVE_CAMERA_ALONG_SPLINE",
                {
                    false,
                    382,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_CAMERA_POSITION_ALONG_SPLINE",
                {
                    false,
                    383,
                    {
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "DECLARE_MISSION_FLAG",
                {
                    true,
                    384,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "DECLARE_MISSION_FLAG_FOR_CONTACT",
                {
                    false,
                    385,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "DECLARE_BASE_BRIEF_ID_FOR_CONTACT",
                {
                    false,
                    386,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_HEALTH_GREATER",
                {
                    true,
                    387,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_HEALTH_GREATER",
                {
                    true,
                    388,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_HEALTH_GREATER",
                {
                    true,
                    389,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_BLIP_FOR_CAR",
                {
                    true,
                    390,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_BLIP_FOR_CHAR",
                {
                    true,
                    391,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_BLIP_FOR_OBJECT",
                {
                    true,
                    392,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_BLIP_FOR_CONTACT_POINT",
                {
                    true,
                    393,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_BLIP_FOR_COORD",
                {
                    true,
                    394,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "CHANGE_BLIP_DISPLAY",
                {
                    true,
                    395,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_ONE_OFF_SOUND",
                {
                    true,
                    396,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_CONTINUOUS_SOUND",
                {
                    true,
                    397,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "REMOVE_SOUND",
                {
                    true,
                    398,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_STUCK_ON_ROOF",
                {
                    true,
                    399,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_UPSIDEDOWN_CAR_CHECK",
                {
                    true,
                    400,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REMOVE_UPSIDEDOWN_CAR_CHECK",
                {
                    true,
                    401,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_WAIT_ON_FOOT",
                {
                    true,
                    402,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_FLEE_ON_FOOT_TILL_SAFE",
                {
                    true,
                    403,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_GUARD_SPOT",
                {
                    true,
                    404,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_GUARD_AREA",
                {
                    false,
                    405,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_WAIT_IN_CAR",
                {
                    false,
                    406,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_AREA_ON_FOOT_2D",
                {
                    true,
                    407,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_AREA_IN_CAR_2D",
                {
                    true,
                    408,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_STOPPED_IN_AREA_2D",
                {
                    true,
                    409,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_STOPPED_IN_AREA_ON_FOOT_2D",
                {
                    true,
                    410,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_STOPPED_IN_AREA_IN_CAR_2D",
                {
                    true,
                    411,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_AREA_ON_FOOT_3D",
                {
                    true,
                    412,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_AREA_IN_CAR_3D",
                {
                    true,
                    413,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_STOPPED_IN_AREA_3D",
                {
                    true,
                    414,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_STOPPED_IN_AREA_ON_FOOT_3D",
                {
                    true,
                    415,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_STOPPED_IN_AREA_IN_CAR_3D",
                {
                    true,
                    416,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_AREA_ON_FOOT_2D",
                {
                    true,
                    417,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_AREA_IN_CAR_2D",
                {
                    true,
                    418,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_STOPPED_IN_AREA_2D",
                {
                    true,
                    419,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_STOPPED_IN_AREA_ON_FOOT_2D",
                {
                    true,
                    420,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_STOPPED_IN_AREA_IN_CAR_2D",
                {
                    true,
                    421,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_AREA_ON_FOOT_3D",
                {
                    true,
                    422,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_AREA_IN_CAR_3D",
                {
                    true,
                    423,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_STOPPED_IN_AREA_3D",
                {
                    true,
                    424,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_STOPPED_IN_AREA_ON_FOOT_3D",
                {
                    true,
                    425,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_STOPPED_IN_AREA_IN_CAR_3D",
                {
                    true,
                    426,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_STOPPED_IN_AREA_2D",
                {
                    true,
                    427,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_STOPPED_IN_AREA_3D",
                {
                    true,
                    428,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CAR_2D",
                {
                    true,
                    429,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_CAR_2D",
                {
                    true,
                    430,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CAR_3D",
                {
                    true,
                    431,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_CAR_3D",
                {
                    true,
                    432,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GIVE_WEAPON_TO_PLAYER",
                {
                    true,
                    433,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GIVE_WEAPON_TO_CHAR",
                {
                    true,
                    434,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GIVE_WEAPON_TO_CAR",
                {
                    false,
                    435,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_CONTROL",
                {
                    true,
                    436,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FORCE_WEATHER",
                {
                    true,
                    437,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FORCE_WEATHER_NOW",
                {
                    true,
                    438,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "RELEASE_WEATHER",
                {
                    true,
                    439,
                    {
                    }
                }
            },
            {
                "SET_CURRENT_PLAYER_WEAPON",
                {
                    true,
                    440,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CURRENT_CHAR_WEAPON",
                {
                    true,
                    441,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CURRENT_CAR_WEAPON",
                {
                    false,
                    442,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_OBJECT_COORDINATES",
                {
                    true,
                    443,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_OBJECT_COORDINATES",
                {
                    true,
                    444,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_GAME_TIMER",
                {
                    true,
                    445,
                    {
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "TURN_CHAR_TO_FACE_COORD",
                {
                    true,
                    446,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "TURN_PLAYER_TO_FACE_COORD",
                {
                    false,
                    447,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "STORE_WANTED_LEVEL",
                {
                    true,
                    448,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_STOPPED",
                {
                    true,
                    449,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "MARK_CHAR_AS_NO_LONGER_NEEDED",
                {
                    true,
                    450,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "MARK_CAR_AS_NO_LONGER_NEEDED",
                {
                    true,
                    451,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "MARK_OBJECT_AS_NO_LONGER_NEEDED",
                {
                    true,
                    452,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DONT_REMOVE_CHAR",
                {
                    true,
                    453,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DONT_REMOVE_CAR",
                {
                    false,
                    454,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DONT_REMOVE_OBJECT",
                {
                    true,
                    455,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CREATE_CHAR_AS_PASSENGER",
                {
                    true,
                    456,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_KILL_CHAR_ON_FOOT",
                {
                    true,
                    457,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_KILL_PLAYER_ON_FOOT",
                {
                    true,
                    458,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_KILL_CHAR_ANY_MEANS",
                {
                    true,
                    459,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_KILL_PLAYER_ANY_MEANS",
                {
                    true,
                    460,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_FLEE_CHAR_ON_FOOT_TILL_SAFE",
                {
                    false,
                    461,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_FLEE_PLAYER_ON_FOOT_TILL_SAFE",
                {
                    true,
                    462,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_FLEE_CHAR_ON_FOOT_ALWAYS",
                {
                    true,
                    463,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_FLEE_PLAYER_ON_FOOT_ALWAYS",
                {
                    true,
                    464,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_GOTO_CHAR_ON_FOOT",
                {
                    true,
                    465,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_GOTO_PLAYER_ON_FOOT",
                {
                    true,
                    466,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_LEAVE_CAR",
                {
                    true,
                    467,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_ENTER_CAR_AS_PASSENGER",
                {
                    true,
                    468,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_ENTER_CAR_AS_DRIVER",
                {
                    true,
                    469,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_FOLLOW_CAR_IN_CAR",
                {
                    false,
                    470,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_FIRE_AT_OBJECT_FROM_VEHICLE",
                {
                    false,
                    471,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_DESTROY_OBJECT",
                {
                    true,
                    472,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_DESTROY_CAR",
                {
                    true,
                    473,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_GOTO_AREA_ON_FOOT",
                {
                    false,
                    474,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_GOTO_AREA_IN_CAR",
                {
                    false,
                    475,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_FOLLOW_CAR_ON_FOOT_WITH_OFFSET",
                {
                    false,
                    476,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_GUARD_ATTACK",
                {
                    false,
                    477,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_AS_LEADER",
                {
                    true,
                    478,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_AS_LEADER",
                {
                    true,
                    479,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LEAVE_GROUP",
                {
                    true,
                    480,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_FOLLOW_ROUTE",
                {
                    true,
                    481,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_ROUTE_POINT",
                {
                    true,
                    482,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_NUMBER_BIG",
                {
                    true,
                    483,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_NUMBER",
                {
                    true,
                    484,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_NUMBER_NOW",
                {
                    true,
                    485,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_NUMBER_SOON",
                {
                    false,
                    486,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_ROADS_ON",
                {
                    true,
                    487,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_ROADS_OFF",
                {
                    true,
                    488,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_NUMBER_OF_PASSENGERS",
                {
                    true,
                    489,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_MAXIMUM_NUMBER_OF_PASSENGERS",
                {
                    true,
                    490,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_DENSITY_MULTIPLIER",
                {
                    true,
                    491,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_HEAVY",
                {
                    true,
                    492,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_CHAR_THREAT_SEARCH",
                {
                    true,
                    493,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ACTIVATE_CRANE",
                {
                    false,
                    494,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "DEACTIVATE_CRANE",
                {
                    false,
                    495,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_MAX_WANTED_LEVEL",
                {
                    true,
                    496,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_IN_AIR_PROPER",
                {
                    true,
                    499,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_UPSIDEDOWN",
                {
                    true,
                    500,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_PLAYER_CHAR",
                {
                    true,
                    501,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "CANCEL_OVERRIDE_RESTART",
                {
                    true,
                    502,
                    {
                    }
                }
            },
            {
                "SET_POLICE_IGNORE_PLAYER",
                {
                    true,
                    503,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_PAGER_MESSAGE_WITH_NUMBER",
                {
                    false,
                    504,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "START_KILL_FRENZY",
                {
                    true,
                    505,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "READ_KILL_FRENZY_STATUS",
                {
                    true,
                    506,
                    {
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SQRT",
                {
                    true,
                    507,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_ANY_MEANS_CAR_2D",
                {
                    true,
                    508,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_ON_FOOT_CAR_2D",
                {
                    true,
                    509,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_IN_CAR_CAR_2D",
                {
                    true,
                    510,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_ANY_MEANS_CAR_3D",
                {
                    true,
                    511,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_ON_FOOT_CAR_3D",
                {
                    true,
                    512,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_PLAYER_IN_CAR_CAR_3D",
                {
                    true,
                    513,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_ANY_MEANS_CAR_2D",
                {
                    true,
                    514,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_ON_FOOT_CAR_2D",
                {
                    true,
                    515,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_IN_CAR_CAR_2D",
                {
                    true,
                    516,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_ANY_MEANS_CAR_3D",
                {
                    true,
                    517,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_ON_FOOT_CAR_3D",
                {
                    true,
                    518,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_IN_CAR_CAR_3D",
                {
                    true,
                    519,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GENERATE_RANDOM_FLOAT_IN_RANGE",
                {
                    true,
                    520,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "GENERATE_RANDOM_INT_IN_RANGE",
                {
                    true,
                    521,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "LOCK_CAR_DOORS",
                {
                    true,
                    522,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "EXPLODE_CAR",
                {
                    true,
                    523,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_EXPLOSION",
                {
                    true,
                    524,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_UPRIGHT",
                {
                    true,
                    525,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "TURN_CHAR_TO_FACE_CHAR",
                {
                    true,
                    526,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "TURN_CHAR_TO_FACE_PLAYER",
                {
                    true,
                    527,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "TURN_PLAYER_TO_FACE_CHAR",
                {
                    true,
                    528,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_GOTO_COORD_ON_FOOT",
                {
                    true,
                    529,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_GOTO_COORD_IN_CAR",
                {
                    false,
                    530,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CREATE_PICKUP",
                {
                    true,
                    531,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "HAS_PICKUP_BEEN_COLLECTED",
                {
                    true,
                    532,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REMOVE_PICKUP",
                {
                    true,
                    533,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TAXI_LIGHTS",
                {
                    true,
                    534,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_BIG_Q",
                {
                    true,
                    535,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_NUMBER_BIG_Q",
                {
                    false,
                    536,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_GARAGE",
                {
                    true,
                    537,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_GARAGE_WITH_CAR_MODEL",
                {
                    false,
                    538,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_TARGET_CAR_FOR_MISSION_GARAGE",
                {
                    true,
                    539,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_IN_MISSION_GARAGE",
                {
                    true,
                    540,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_FREE_BOMBS",
                {
                    false,
                    541,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_POWERPOINT",
                {
                    false,
                    542,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_ALL_TAXI_LIGHTS",
                {
                    false,
                    543,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_ARMED_WITH_ANY_BOMB",
                {
                    false,
                    544,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "APPLY_BRAKES_TO_PLAYERS_CAR",
                {
                    true,
                    545,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_HEALTH",
                {
                    true,
                    546,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_HEALTH",
                {
                    true,
                    547,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_HEALTH",
                {
                    true,
                    548,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_PLAYER_HEALTH",
                {
                    true,
                    549,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_CHAR_HEALTH",
                {
                    true,
                    550,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_CAR_HEALTH",
                {
                    true,
                    551,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_ARMED_WITH_BOMB",
                {
                    false,
                    552,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CHANGE_CAR_COLOUR",
                {
                    true,
                    553,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_PED_ROADS_ON",
                {
                    true,
                    554,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_PED_ROADS_OFF",
                {
                    true,
                    555,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "CHAR_LOOK_AT_CHAR_ALWAYS",
                {
                    true,
                    556,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CHAR_LOOK_AT_PLAYER_ALWAYS",
                {
                    true,
                    557,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PLAYER_LOOK_AT_CHAR_ALWAYS",
                {
                    true,
                    558,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "STOP_CHAR_LOOKING",
                {
                    true,
                    559,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "STOP_PLAYER_LOOKING",
                {
                    true,
                    560,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_HELICOPTER",
                {
                    false,
                    561,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_GANG_ATTITUDE",
                {
                    false,
                    562,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_GANG_GANG_ATTITUDE",
                {
                    false,
                    563,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_GANG_PLAYER_ATTITUDE",
                {
                    false,
                    564,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_GANG_PED_MODELS",
                {
                    true,
                    565,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_GANG_CAR_MODEL",
                {
                    true,
                    566,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_GANG_WEAPONS",
                {
                    true,
                    567,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_RUN_TO_AREA",
                {
                    false,
                    568,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_RUN_TO_COORD",
                {
                    true,
                    569,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_TOUCHING_OBJECT_ON_FOOT",
                {
                    false,
                    570,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_TOUCHING_OBJECT_ON_FOOT",
                {
                    false,
                    571,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOAD_SPECIAL_CHARACTER",
                {
                    true,
                    572,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "HAS_SPECIAL_CHARACTER_LOADED",
                {
                    true,
                    573,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FLASH_CAR",
                {
                    false,
                    574,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FLASH_CHAR",
                {
                    false,
                    575,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FLASH_OBJECT",
                {
                    false,
                    576,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_REMOTE_MODE",
                {
                    true,
                    577,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ARM_CAR_WITH_BOMB",
                {
                    false,
                    578,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_PERSONALITY",
                {
                    true,
                    579,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CUTSCENE_OFFSET",
                {
                    true,
                    580,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_ANIM_GROUP_FOR_CHAR",
                {
                    true,
                    581,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_ANIM_GROUP_FOR_PLAYER",
                {
                    false,
                    582,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REQUEST_MODEL",
                {
                    true,
                    583,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_MODEL_LOADED",
                {
                    true,
                    584,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "MARK_MODEL_AS_NO_LONGER_NEEDED",
                {
                    true,
                    585,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GRAB_PHONE",
                {
                    true,
                    586,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_REPEATED_PHONE_MESSAGE",
                {
                    false,
                    587,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_PHONE_MESSAGE",
                {
                    false,
                    588,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "HAS_PHONE_DISPLAYED_MESSAGE",
                {
                    false,
                    589,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "TURN_PHONE_OFF",
                {
                    true,
                    590,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DRAW_CORONA",
                {
                    true,
                    591,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DRAW_LIGHT",
                {
                    false,
                    592,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "STORE_WEATHER",
                {
                    false,
                    593,
                    {
                    }
                }
            },
            {
                "RESTORE_WEATHER",
                {
                    false,
                    594,
                    {
                    }
                }
            },
            {
                "STORE_CLOCK",
                {
                    true,
                    595,
                    {
                    }
                }
            },
            {
                "RESTORE_CLOCK",
                {
                    true,
                    596,
                    {
                    }
                }
            },
            {
                "RESTART_CRITICAL_MISSION",
                {
                    false,
                    597,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_PLAYING",
                {
                    true,
                    598,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_NO_OBJ",
                {
                    false,
                    599,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_WAIT_ON_FOOT",
                {
                    false,
                    600,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_FLEE_ON_FOOT_TILL_SAFE",
                {
                    false,
                    601,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_GUARD_SPOT",
                {
                    false,
                    602,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_GUARD_AREA",
                {
                    false,
                    603,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_WAIT_IN_CAR",
                {
                    false,
                    604,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_KILL_CHAR_ON_FOOT",
                {
                    false,
                    605,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_KILL_PLAYER_ON_FOOT",
                {
                    false,
                    606,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_KILL_CHAR_ANY_MEANS",
                {
                    false,
                    607,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_KILL_PLAYER_ANY_MEANS",
                {
                    false,
                    608,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_FLEE_CHAR_ON_FOOT_TILL_SAFE",
                {
                    false,
                    609,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_FLEE_PLAYER_ON_FOOT_TILL_SAFE",
                {
                    false,
                    610,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_FLEE_CHAR_ON_FOOT_ALWAYS",
                {
                    false,
                    611,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_FLEE_PLAYER_ON_FOOT_ALWAYS",
                {
                    false,
                    612,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_GOTO_CHAR_ON_FOOT",
                {
                    false,
                    613,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_GOTO_PLAYER_ON_FOOT",
                {
                    false,
                    614,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_LEAVE_CAR",
                {
                    false,
                    615,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_ENTER_CAR_AS_PASSENGER",
                {
                    false,
                    616,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_ENTER_CAR_AS_DRIVER",
                {
                    false,
                    617,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_FOLLOW_CAR_IN_CAR",
                {
                    false,
                    618,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_FIRE_AT_OBJECT_FROM_VEHICLE",
                {
                    false,
                    619,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_DESTROY_OBJECT",
                {
                    false,
                    620,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_DESTROY_CAR",
                {
                    false,
                    621,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_GOTO_AREA_ON_FOOT",
                {
                    false,
                    622,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_GOTO_AREA_IN_CAR",
                {
                    false,
                    623,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_FOLLOW_CAR_ON_FOOT_WITH_OFFSET",
                {
                    false,
                    624,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_GUARD_ATTACK",
                {
                    false,
                    625,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_FOLLOW_ROUTE",
                {
                    false,
                    626,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_GOTO_COORD_ON_FOOT",
                {
                    false,
                    627,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_GOTO_COORD_IN_CAR",
                {
                    false,
                    628,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_RUN_TO_AREA",
                {
                    false,
                    629,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_RUN_TO_COORD",
                {
                    false,
                    630,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_PEDS_IN_AREA_TO_COLL",
                {
                    false,
                    631,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_PEDS_IN_VEHICLE_TO_COLL",
                {
                    false,
                    632,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_COLL",
                {
                    false,
                    633,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_COLL_IN_CARS",
                {
                    false,
                    634,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_COLL_ANY_MEANS_2D",
                {
                    false,
                    635,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_COLL_ON_FOOT_2D",
                {
                    false,
                    636,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_COLL_IN_CAR_2D",
                {
                    false,
                    637,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_COLL_ANY_MEANS_2D",
                {
                    false,
                    638,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_COLL_ON_FOOT_2D",
                {
                    false,
                    639,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_STOPPED_COLL_IN_CAR_2D",
                {
                    false,
                    640,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_COLL_ANY_MEANS_CHAR_2D",
                {
                    false,
                    641,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_COLL_ON_FOOT_CHAR_2D",
                {
                    false,
                    642,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_COLL_IN_CAR_CHAR_2D",
                {
                    false,
                    643,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_COLL_ANY_MEANS_CAR_2D",
                {
                    false,
                    644,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_COLL_ON_FOOT_CAR_2D",
                {
                    false,
                    645,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_COLL_IN_CAR_CAR_2D",
                {
                    false,
                    646,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_COLL_ANY_MEANS_PLAYER_2D",
                {
                    false,
                    647,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_COLL_ON_FOOT_PLAYER_2D",
                {
                    false,
                    648,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_COLL_IN_CAR_PLAYER_2D",
                {
                    false,
                    649,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_COLL_IN_AREA_2D",
                {
                    false,
                    650,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_COLL_IN_AREA_ON_FOOT_2D",
                {
                    false,
                    651,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_COLL_IN_AREA_IN_CAR_2D",
                {
                    false,
                    652,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_COLL_STOPPED_IN_AREA_2D",
                {
                    false,
                    653,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_COLL_STOPPED_IN_AREA_ON_FOOT_2D",
                {
                    false,
                    654,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_COLL_STOPPED_IN_AREA_IN_CAR_2D",
                {
                    false,
                    655,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_NUMBER_OF_PEDS_IN_COLL",
                {
                    false,
                    656,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_HEED_THREATS",
                {
                    true,
                    657,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_HEED_THREATS",
                {
                    false,
                    658,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_CONTROLLER_MODE",
                {
                    true,
                    659,
                    {
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CAN_RESPRAY_CAR",
                {
                    true,
                    660,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_TAXI",
                {
                    false,
                    661,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "UNLOAD_SPECIAL_CHARACTER",
                {
                    true,
                    662,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "RESET_NUM_OF_MODELS_KILLED_BY_PLAYER",
                {
                    true,
                    663,
                    {
                    }
                }
            },
            {
                "GET_NUM_OF_MODELS_KILLED_BY_PLAYER",
                {
                    true,
                    664,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ACTIVATE_GARAGE",
                {
                    false,
                    665,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_TAXI_TIMER",
                {
                    false,
                    666,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CREATE_OBJECT_NO_OFFSET",
                {
                    true,
                    667,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_BOAT",
                {
                    false,
                    668,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_GOTO_AREA_ANY_MEANS",
                {
                    false,
                    669,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_GOTO_AREA_ANY_MEANS",
                {
                    false,
                    670,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_STOPPED",
                {
                    true,
                    671,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_STOPPED",
                {
                    true,
                    672,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "MESSAGE_WAIT",
                {
                    false,
                    673,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_PARTICLE_EFFECT",
                {
                    false,
                    674,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_WIDESCREEN",
                {
                    true,
                    675,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_SPRITE_BLIP_FOR_CAR",
                {
                    false,
                    676,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_SPRITE_BLIP_FOR_CHAR",
                {
                    false,
                    677,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_SPRITE_BLIP_FOR_OBJECT",
                {
                    false,
                    678,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_SPRITE_BLIP_FOR_CONTACT_POINT",
                {
                    true,
                    679,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_SPRITE_BLIP_FOR_COORD",
                {
                    true,
                    680,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_ONLY_DAMAGED_BY_PLAYER",
                {
                    true,
                    681,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_ONLY_DAMAGED_BY_PLAYER",
                {
                    true,
                    682,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_PROOFS",
                {
                    true,
                    683,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_PROOFS",
                {
                    true,
                    684,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_ANGLED_AREA_2D",
                {
                    true,
                    685,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_ANGLED_AREA_ON_FOOT_2D",
                {
                    true,
                    686,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_ANGLED_AREA_IN_CAR_2D",
                {
                    true,
                    687,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_STOPPED_IN_ANGLED_AREA_2D",
                {
                    true,
                    688,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_STOPPED_IN_ANGLED_AREA_ON_FOOT_2D",
                {
                    true,
                    689,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_STOPPED_IN_ANGLED_AREA_IN_CAR_2D",
                {
                    true,
                    690,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_ANGLED_AREA_3D",
                {
                    true,
                    691,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_ANGLED_AREA_ON_FOOT_3D",
                {
                    true,
                    692,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_ANGLED_AREA_IN_CAR_3D",
                {
                    true,
                    693,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_STOPPED_IN_ANGLED_AREA_3D",
                {
                    true,
                    694,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_STOPPED_IN_ANGLED_AREA_ON_FOOT_3D",
                {
                    true,
                    695,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_STOPPED_IN_ANGLED_AREA_IN_CAR_3D",
                {
                    true,
                    696,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DEACTIVATE_GARAGE",
                {
                    false,
                    697,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_NUMBER_OF_CARS_COLLECTED_BY_GARAGE",
                {
                    false,
                    698,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "HAS_CAR_BEEN_TAKEN_TO_GARAGE",
                {
                    false,
                    699,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_SWAT_REQUIRED",
                {
                    false,
                    700,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_FBI_REQUIRED",
                {
                    false,
                    701,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_ARMY_REQUIRED",
                {
                    false,
                    702,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_IN_WATER",
                {
                    true,
                    703,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_CLOSEST_CHAR_NODE",
                {
                    true,
                    704,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_CLOSEST_CAR_NODE",
                {
                    true,
                    705,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "CAR_GOTO_COORDINATES_ACCURATE",
                {
                    true,
                    706,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "START_PACMAN_RACE",
                {
                    false,
                    707,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "START_PACMAN_RECORD",
                {
                    false,
                    708,
                    {
                    }
                }
            },
            {
                "GET_NUMBER_OF_POWER_PILLS_EATEN",
                {
                    false,
                    709,
                    {
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_PACMAN",
                {
                    false,
                    710,
                    {
                    }
                }
            },
            {
                "START_PACMAN_SCRAMBLE",
                {
                    false,
                    711,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_NUMBER_OF_POWER_PILLS_CARRIED",
                {
                    false,
                    712,
                    {
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_NUMBER_OF_POWER_PILLS_CARRIED",
                {
                    false,
                    713,
                    {
                    }
                }
            },
            {
                "IS_CAR_ON_SCREEN",
                {
                    true,
                    714,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_ON_SCREEN",
                {
                    true,
                    715,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_OBJECT_ON_SCREEN",
                {
                    true,
                    716,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GOSUB_FILE",
                {
                    false,
                    717,
                    {
                        { ArgType::Label, false, true, false, false,{} },
                        { ArgType::Label, false, true, false, false,{} },
                    }
                }
            },
            {
                "GET_GROUND_Z_FOR_3D_COORD",
                {
                    true,
                    718,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "START_SCRIPT_FIRE",
                {
                    true,
                    719,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_SCRIPT_FIRE_EXTINGUISHED",
                {
                    true,
                    720,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REMOVE_SCRIPT_FIRE",
                {
                    true,
                    721,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COMEDY_CONTROLS",
                {
                    false,
                    722,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "BOAT_GOTO_COORDS",
                {
                    true,
                    723,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "BOAT_STOP",
                {
                    true,
                    724,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_SHOOTING_IN_AREA",
                {
                    true,
                    725,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_SHOOTING_IN_AREA",
                {
                    false,
                    726,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CURRENT_PLAYER_WEAPON",
                {
                    true,
                    727,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CURRENT_CHAR_WEAPON",
                {
                    true,
                    728,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_NUMBER_OF_POWER_PILLS_EATEN",
                {
                    false,
                    729,
                    {
                    }
                }
            },
            {
                "ADD_POWER_PILL",
                {
                    false,
                    730,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_BOAT_CRUISE_SPEED",
                {
                    true,
                    731,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_RANDOM_CHAR_IN_AREA",
                {
                    false,
                    732,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_RANDOM_CHAR_IN_ZONE",
                {
                    true,
                    733,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_TAXI",
                {
                    true,
                    734,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_SHOOTING",
                {
                    true,
                    735,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_SHOOTING",
                {
                    true,
                    736,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CREATE_MONEY_PICKUP",
                {
                    true,
                    737,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_ACCURACY",
                {
                    true,
                    738,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_CAR_SPEED",
                {
                    true,
                    739,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "LOAD_CUTSCENE",
                {
                    true,
                    740,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "CREATE_CUTSCENE_OBJECT",
                {
                    true,
                    741,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CUTSCENE_ANIM",
                {
                    true,
                    742,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "START_CUTSCENE",
                {
                    true,
                    743,
                    {
                    }
                }
            },
            {
                "GET_CUTSCENE_TIME",
                {
                    true,
                    744,
                    {
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "HAS_CUTSCENE_FINISHED",
                {
                    true,
                    745,
                    {
                    }
                }
            },
            {
                "CLEAR_CUTSCENE",
                {
                    true,
                    746,
                    {
                    }
                }
            },
            {
                "RESTORE_CAMERA_JUMPCUT",
                {
                    true,
                    747,
                    {
                    }
                }
            },
            {
                "CREATE_COLLECTABLE1",
                {
                    true,
                    748,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLLECTABLE1_TOTAL",
                {
                    true,
                    749,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PROJECTILE_IN_AREA",
                {
                    false,
                    750,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "DESTROY_PROJECTILES_IN_AREA",
                {
                    false,
                    751,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "DROP_MINE",
                {
                    false,
                    752,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "DROP_NAUTICAL_MINE",
                {
                    false,
                    753,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_MODEL",
                {
                    true,
                    754,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOAD_SPECIAL_MODEL",
                {
                    true,
                    755,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "CREATE_CUTSCENE_HEAD",
                {
                    false,
                    756,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CUTSCENE_HEAD_ANIM",
                {
                    false,
                    757,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SIN",
                {
                    true,
                    758,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "COS",
                {
                    true,
                    759,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_CAR_FORWARD_X",
                {
                    true,
                    760,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_CAR_FORWARD_Y",
                {
                    true,
                    761,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "CHANGE_GARAGE_TYPE",
                {
                    true,
                    762,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ACTIVATE_CRUSHER_CRANE",
                {
                    false,
                    763,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_2_NUMBERS",
                {
                    false,
                    764,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_2_NUMBERS_NOW",
                {
                    true,
                    765,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_2_NUMBERS_SOON",
                {
                    false,
                    766,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_3_NUMBERS",
                {
                    true,
                    767,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_3_NUMBERS_NOW",
                {
                    false,
                    768,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_3_NUMBERS_SOON",
                {
                    false,
                    769,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_4_NUMBERS",
                {
                    true,
                    770,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_4_NUMBERS_NOW",
                {
                    false,
                    771,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_4_NUMBERS_SOON",
                {
                    false,
                    772,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_5_NUMBERS",
                {
                    false,
                    773,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_5_NUMBERS_NOW",
                {
                    false,
                    774,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_5_NUMBERS_SOON",
                {
                    false,
                    775,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_6_NUMBERS",
                {
                    true,
                    776,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_6_NUMBERS_NOW",
                {
                    false,
                    777,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_6_NUMBERS_SOON",
                {
                    false,
                    778,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_FOLLOW_CHAR_IN_FORMATION",
                {
                    false,
                    779,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PLAYER_MADE_PROGRESS",
                {
                    true,
                    780,
                    {
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_PROGRESS_TOTAL",
                {
                    true,
                    781,
                    {
                        { ArgType::Integer, false, true, false, false,{} },
                    }
                }
            },
            {
                "REGISTER_JUMP_DISTANCE",
                {
                    true,
                    782,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_JUMP_HEIGHT",
                {
                    true,
                    783,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_JUMP_FLIPS",
                {
                    true,
                    784,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_JUMP_SPINS",
                {
                    true,
                    785,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_JUMP_STUNT",
                {
                    true,
                    786,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_UNIQUE_JUMP_FOUND",
                {
                    true,
                    787,
                    {
                    }
                }
            },
            {
                "SET_UNIQUE_JUMPS_TOTAL",
                {
                    true,
                    788,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_PASSENGER_DROPPED_OFF_TAXI",
                {
                    true,
                    789,
                    {
                    }
                }
            },
            {
                "REGISTER_MONEY_MADE_TAXI",
                {
                    true,
                    790,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_MISSION_GIVEN",
                {
                    true,
                    791,
                    {
                    }
                }
            },
            {
                "REGISTER_MISSION_PASSED",
                {
                    true,
                    792,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_CHAR_RUNNING",
                {
                    true,
                    793,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REMOVE_ALL_SCRIPT_FIRES",
                {
                    true,
                    794,
                    {
                    }
                }
            },
            {
                "IS_FIRST_CAR_COLOUR",
                {
                    false,
                    795,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_SECOND_CAR_COLOUR",
                {
                    false,
                    796,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_CHAR_BEEN_DAMAGED_BY_WEAPON",
                {
                    true,
                    797,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_CAR_BEEN_DAMAGED_BY_WEAPON",
                {
                    true,
                    798,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_CHARS_GROUP",
                {
                    true,
                    799,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_PLAYERS_GROUP",
                {
                    true,
                    800,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "EXPLODE_CHAR_HEAD",
                {
                    true,
                    801,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "EXPLODE_PLAYER_HEAD",
                {
                    true,
                    802,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ANCHOR_BOAT",
                {
                    true,
                    803,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_ZONE_GROUP",
                {
                    true,
                    804,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "START_CAR_FIRE",
                {
                    true,
                    805,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "START_CHAR_FIRE",
                {
                    true,
                    806,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_RANDOM_CAR_OF_TYPE_IN_AREA",
                {
                    true,
                    807,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_RANDOM_CAR_OF_TYPE_IN_ZONE",
                {
                    false,
                    808,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "HAS_RESPRAY_HAPPENED",
                {
                    true,
                    809,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAMERA_ZOOM",
                {
                    true,
                    810,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CREATE_PICKUP_WITH_AMMO",
                {
                    true,
                    811,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_RAM_CAR",
                {
                    true,
                    812,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_BLOCK_CAR",
                {
                    false,
                    813,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_CATCH_TRAIN",
                {
                    false,
                    814,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_COLL_OBJ_CATCH_TRAIN",
                {
                    false,
                    815,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_NEVER_GETS_TIRED",
                {
                    true,
                    816,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_FAST_RELOAD",
                {
                    true,
                    817,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_BLEEDING",
                {
                    true,
                    818,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_FUNNY_SUSPENSION",
                {
                    false,
                    819,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_BIG_WHEELS",
                {
                    false,
                    820,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_FREE_RESPRAYS",
                {
                    true,
                    821,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_VISIBLE",
                {
                    true,
                    822,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_VISIBLE",
                {
                    true,
                    823,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_VISIBLE",
                {
                    false,
                    824,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_AREA_OCCUPIED",
                {
                    true,
                    825,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "START_DRUG_RUN",
                {
                    false,
                    826,
                    {
                    }
                }
            },
            {
                "HAS_DRUG_RUN_BEEN_COMPLETED",
                {
                    false,
                    827,
                    {
                    }
                }
            },
            {
                "HAS_DRUG_PLANE_BEEN_SHOT_DOWN",
                {
                    false,
                    828,
                    {
                    }
                }
            },
            {
                "SAVE_PLAYER_FROM_FIRES",
                {
                    false,
                    829,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DISPLAY_TEXT",
                {
                    true,
                    830,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_TEXT_SCALE",
                {
                    true,
                    831,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TEXT_COLOUR",
                {
                    true,
                    832,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TEXT_JUSTIFY",
                {
                    true,
                    833,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TEXT_CENTRE",
                {
                    true,
                    834,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TEXT_WRAPX",
                {
                    true,
                    835,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TEXT_CENTRE_SIZE",
                {
                    false,
                    836,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TEXT_BACKGROUND",
                {
                    true,
                    837,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TEXT_BACKGROUND_COLOUR",
                {
                    false,
                    838,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TEXT_BACKGROUND_ONLY_TEXT",
                {
                    false,
                    839,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TEXT_PROPORTIONAL",
                {
                    true,
                    840,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TEXT_FONT",
                {
                    false,
                    841,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "INDUSTRIAL_PASSED",
                {
                    false,
                    842,
                    {
                    }
                }
            },
            {
                "COMMERCIAL_PASSED",
                {
                    false,
                    843,
                    {
                    }
                }
            },
            {
                "SUBURBAN_PASSED",
                {
                    false,
                    844,
                    {
                    }
                }
            },
            {
                "ROTATE_OBJECT",
                {
                    true,
                    845,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SLIDE_OBJECT",
                {
                    true,
                    846,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REMOVE_CHAR_ELEGANTLY",
                {
                    true,
                    847,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_STAY_IN_SAME_PLACE",
                {
                    true,
                    848,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_NASTY_GAME",
                {
                    false,
                    849,
                    {
                    }
                }
            },
            {
                "UNDRESS_CHAR",
                {
                    true,
                    850,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "DRESS_CHAR",
                {
                    true,
                    851,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "START_CHASE_SCENE",
                {
                    false,
                    852,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "STOP_CHASE_SCENE",
                {
                    false,
                    853,
                    {
                    }
                }
            },
            {
                "IS_EXPLOSION_IN_AREA",
                {
                    false,
                    854,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_EXPLOSION_IN_ZONE",
                {
                    false,
                    855,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "START_DRUG_DROP_OFF",
                {
                    false,
                    856,
                    {
                    }
                }
            },
            {
                "HAS_DROP_OFF_PLANE_BEEN_SHOT_DOWN",
                {
                    false,
                    857,
                    {
                    }
                }
            },
            {
                "FIND_DROP_OFF_PLANE_COORDINATES",
                {
                    false,
                    858,
                    {
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "CREATE_FLOATING_PACKAGE",
                {
                    false,
                    859,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "PLACE_OBJECT_RELATIVE_TO_CAR",
                {
                    true,
                    860,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "MAKE_OBJECT_TARGETTABLE",
                {
                    true,
                    861,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_ARMOUR_TO_PLAYER",
                {
                    true,
                    862,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_ARMOUR_TO_CHAR",
                {
                    true,
                    863,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "OPEN_GARAGE",
                {
                    true,
                    864,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLOSE_GARAGE",
                {
                    true,
                    865,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "WARP_CHAR_FROM_CAR_TO_COORD",
                {
                    true,
                    866,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_VISIBILITY_OF_CLOSEST_OBJECT_OF_TYPE",
                {
                    true,
                    867,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_CHAR_SPOTTED_CHAR",
                {
                    false,
                    868,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_HAIL_TAXI",
                {
                    true,
                    869,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_OBJECT_BEEN_DAMAGED",
                {
                    true,
                    870,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "START_KILL_FRENZY_HEADSHOT",
                {
                    false,
                    871,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ACTIVATE_MILITARY_CRANE",
                {
                    false,
                    872,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "WARP_PLAYER_INTO_CAR",
                {
                    true,
                    873,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "WARP_CHAR_INTO_CAR",
                {
                    true,
                    874,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_CAR_RADIO",
                {
                    false,
                    875,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_AUDIO_STREAM",
                {
                    false,
                    876,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_2_NUMBERS_BIG",
                {
                    true,
                    877,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_3_NUMBERS_BIG",
                {
                    false,
                    878,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_4_NUMBERS_BIG",
                {
                    false,
                    879,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_5_NUMBERS_BIG",
                {
                    false,
                    880,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_WITH_6_NUMBERS_BIG",
                {
                    false,
                    881,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_WAIT_STATE",
                {
                    true,
                    882,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAMERA_BEHIND_PLAYER",
                {
                    true,
                    883,
                    {
                    }
                }
            },
            {
                "SET_MOTION_BLUR",
                {
                    false,
                    884,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_STRING_IN_STRING",
                {
                    false,
                    885,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CREATE_RANDOM_CHAR",
                {
                    true,
                    886,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_STEAL_ANY_CAR",
                {
                    true,
                    887,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_2_REPEATED_PHONE_MESSAGES",
                {
                    false,
                    888,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_2_PHONE_MESSAGES",
                {
                    false,
                    889,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_3_REPEATED_PHONE_MESSAGES",
                {
                    false,
                    890,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_3_PHONE_MESSAGES",
                {
                    false,
                    891,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_4_REPEATED_PHONE_MESSAGES",
                {
                    false,
                    892,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_4_PHONE_MESSAGES",
                {
                    false,
                    893,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_SNIPER_BULLET_IN_AREA",
                {
                    true,
                    894,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "GIVE_PLAYER_DETONATOR",
                {
                    false,
                    895,
                    {
                    }
                }
            },
            {
                "SET_COLL_OBJ_STEAL_ANY_CAR",
                {
                    false,
                    896,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_OBJECT_VELOCITY",
                {
                    true,
                    897,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_OBJECT_COLLISION",
                {
                    true,
                    898,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_ICECREAM_JINGLE_ON",
                {
                    true,
                    899,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_STRING_IN_STRING_NOW",
                {
                    true,
                    900,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_STRING_IN_STRING_SOON",
                {
                    false,
                    901,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_5_REPEATED_PHONE_MESSAGES",
                {
                    false,
                    902,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_5_PHONE_MESSAGES",
                {
                    false,
                    903,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_6_REPEATED_PHONE_MESSAGES",
                {
                    false,
                    904,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_6_PHONE_MESSAGES",
                {
                    false,
                    905,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_POINT_OBSCURED_BY_A_MISSION_ENTITY",
                {
                    true,
                    906,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOAD_ALL_MODELS_NOW",
                {
                    true,
                    907,
                    {
                    }
                }
            },
            {
                "ADD_TO_OBJECT_VELOCITY",
                {
                    true,
                    908,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "DRAW_SPRITE",
                {
                    true,
                    909,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DRAW_RECT",
                {
                    true,
                    910,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOAD_SPRITE",
                {
                    true,
                    911,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "LOAD_TEXTURE_DICTIONARY",
                {
                    true,
                    912,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "REMOVE_TEXTURE_DICTIONARY",
                {
                    true,
                    913,
                    {
                    }
                }
            },
            {
                "SET_OBJECT_DYNAMIC",
                {
                    true,
                    914,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_ANIM_SPEED",
                {
                    false,
                    915,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "PLAY_MISSION_PASSED_TUNE",
                {
                    true,
                    916,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_AREA",
                {
                    true,
                    917,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FREEZE_ONSCREEN_TIMER",
                {
                    true,
                    918,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_CAR_SIREN",
                {
                    true,
                    919,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_PED_ROADS_ON_ANGLED",
                {
                    false,
                    920,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_PED_ROADS_OFF_ANGLED",
                {
                    false,
                    921,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_ROADS_ON_ANGLED",
                {
                    false,
                    922,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_ROADS_OFF_ANGLED",
                {
                    false,
                    923,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_WATERTIGHT",
                {
                    true,
                    924,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_MOVING_PARTICLE_EFFECT",
                {
                    true,
                    925,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_CANT_BE_DRAGGED_OUT",
                {
                    true,
                    926,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "TURN_CAR_TO_FACE_COORD",
                {
                    true,
                    927,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CRANE_LIFTING_CAR",
                {
                    false,
                    928,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DRAW_SPHERE",
                {
                    true,
                    929,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_STATUS",
                {
                    true,
                    930,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_MALE",
                {
                    true,
                    931,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SCRIPT_NAME",
                {
                    true,
                    932,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "CHANGE_GARAGE_TYPE_WITH_CAR_MODEL",
                {
                    false,
                    933,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FIND_DRUG_PLANE_COORDINATES",
                {
                    false,
                    934,
                    {
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "SAVE_INT_TO_DEBUG_FILE",
                {
                    true,
                    935,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SAVE_FLOAT_TO_DEBUG_FILE",
                {
                    true,
                    936,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SAVE_NEWLINE_TO_DEBUG_FILE",
                {
                    true,
                    937,
                    {
                    }
                }
            },
            {
                "POLICE_RADIO_MESSAGE",
                {
                    true,
                    938,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_STRONG",
                {
                    true,
                    939,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REMOVE_ROUTE",
                {
                    true,
                    940,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_RUBBISH",
                {
                    true,
                    941,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REMOVE_PARTICLE_EFFECTS_IN_AREA",
                {
                    true,
                    942,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_STREAMING",
                {
                    true,
                    943,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_GARAGE_OPEN",
                {
                    true,
                    944,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_GARAGE_CLOSED",
                {
                    true,
                    945,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "START_CATALINA_HELI",
                {
                    false,
                    946,
                    {
                    }
                }
            },
            {
                "CATALINA_HELI_TAKE_OFF",
                {
                    false,
                    947,
                    {
                    }
                }
            },
            {
                "REMOVE_CATALINA_HELI",
                {
                    false,
                    948,
                    {
                    }
                }
            },
            {
                "HAS_CATALINA_HELI_BEEN_SHOT_DOWN",
                {
                    false,
                    949,
                    {
                    }
                }
            },
            {
                "SWAP_NEAREST_BUILDING_MODEL",
                {
                    true,
                    950,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_WORLD_PROCESSING",
                {
                    true,
                    951,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REMOVE_ALL_PLAYER_WEAPONS",
                {
                    true,
                    952,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GRAB_CATALINA_HELI",
                {
                    false,
                    953,
                    {
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_AREA_OF_CARS",
                {
                    true,
                    954,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_ROTATING_GARAGE_DOOR",
                {
                    true,
                    955,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_SPHERE",
                {
                    true,
                    956,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "REMOVE_SPHERE",
                {
                    true,
                    957,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CATALINA_HELI_FLY_AWAY",
                {
                    false,
                    958,
                    {
                    }
                }
            },
            {
                "SET_EVERYONE_IGNORE_PLAYER",
                {
                    true,
                    959,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "STORE_CAR_CHAR_IS_IN_NO_SAVE",
                {
                    true,
                    960,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "STORE_CAR_PLAYER_IS_IN_NO_SAVE",
                {
                    true,
                    961,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_PHONE_DISPLAYING_MESSAGE",
                {
                    false,
                    962,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DISPLAY_ONSCREEN_TIMER_WITH_STRING",
                {
                    true,
                    963,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "DISPLAY_ONSCREEN_COUNTER_WITH_STRING",
                {
                    true,
                    964,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "CREATE_RANDOM_CAR_FOR_CAR_PARK",
                {
                    true,
                    965,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_COLLISION_IN_MEMORY",
                {
                    false,
                    966,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_WANTED_MULTIPLIER",
                {
                    true,
                    967,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAMERA_IN_FRONT_OF_PLAYER",
                {
                    true,
                    968,
                    {
                    }
                }
            },
            {
                "IS_CAR_VISIBLY_DAMAGED",
                {
                    false,
                    969,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DOES_OBJECT_EXIST",
                {
                    true,
                    970,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOAD_SCENE",
                {
                    true,
                    971,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_STUCK_CAR_CHECK",
                {
                    true,
                    972,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REMOVE_STUCK_CAR_CHECK",
                {
                    true,
                    973,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_STUCK",
                {
                    true,
                    974,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOAD_MISSION_AUDIO",
                {
                    true,
                    975,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "HAS_MISSION_AUDIO_LOADED",
                {
                    true,
                    976,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PLAY_MISSION_AUDIO",
                {
                    true,
                    977,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_MISSION_AUDIO_FINISHED",
                {
                    true,
                    978,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_CLOSEST_CAR_NODE_WITH_HEADING",
                {
                    true,
                    979,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "HAS_IMPORT_GARAGE_SLOT_BEEN_FILLED",
                {
                    true,
                    980,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_THIS_PRINT",
                {
                    true,
                    981,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "CLEAR_THIS_BIG_PRINT",
                {
                    true,
                    982,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_MISSION_AUDIO_POSITION",
                {
                    true,
                    983,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "ACTIVATE_SAVE_MENU",
                {
                    true,
                    984,
                    {
                    }
                }
            },
            {
                "HAS_SAVE_GAME_FINISHED",
                {
                    true,
                    985,
                    {
                    }
                }
            },
            {
                "NO_SPECIAL_CAMERA_FOR_THIS_GARAGE",
                {
                    true,
                    986,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_BLIP_FOR_PICKUP_OLD",
                {
                    false,
                    987,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_BLIP_FOR_PICKUP",
                {
                    true,
                    988,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_SPRITE_BLIP_FOR_PICKUP",
                {
                    false,
                    989,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_PED_DENSITY_MULTIPLIER",
                {
                    true,
                    990,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "FORCE_RANDOM_PED_TYPE",
                {
                    true,
                    991,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TEXT_DRAW_BEFORE_FADE",
                {
                    false,
                    992,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_COLLECTABLE1S_COLLECTED",
                {
                    true,
                    993,
                    {
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_LEAVE_ANY_CAR",
                {
                    true,
                    994,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_SPRITES_DRAW_BEFORE_FADE",
                {
                    true,
                    995,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TEXT_RIGHT_JUSTIFY",
                {
                    true,
                    996,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_HELP",
                {
                    true,
                    997,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "CLEAR_HELP",
                {
                    true,
                    998,
                    {
                    }
                }
            },
            {
                "FLASH_HUD_OBJECT",
                {
                    true,
                    999,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FLASH_RADAR_BLIP",
                {
                    false,
                    1000,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_CONTROL",
                {
                    false,
                    1001,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_GENERATE_CARS_AROUND_CAMERA",
                {
                    true,
                    1002,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_SMALL_PRINTS",
                {
                    true,
                    1003,
                    {
                    }
                }
            },
            {
                "HAS_MILITARY_CRANE_COLLECTED_ALL_CARS",
                {
                    false,
                    1004,
                    {
                    }
                }
            },
            {
                "SET_UPSIDEDOWN_CAR_NOT_DAMAGED",
                {
                    true,
                    1005,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CAN_PLAYER_START_MISSION",
                {
                    true,
                    1006,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "MAKE_PLAYER_SAFE_FOR_CUTSCENE",
                {
                    true,
                    1007,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "USE_TEXT_COMMANDS",
                {
                    true,
                    1008,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_THREAT_FOR_PED_TYPE",
                {
                    true,
                    1009,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_THREAT_FOR_PED_TYPE",
                {
                    true,
                    1010,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_CAR_COLOURS",
                {
                    true,
                    1011,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_ALL_CARS_CAN_BE_DAMAGED",
                {
                    true,
                    1012,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_CAN_BE_DAMAGED",
                {
                    true,
                    1013,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "MAKE_PLAYER_UNSAFE",
                {
                    false,
                    1014,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOAD_COLLISION",
                {
                    false,
                    1015,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_BODY_CAST_HEALTH",
                {
                    false,
                    1016,
                    {
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CHARS_CHATTING",
                {
                    true,
                    1017,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "MAKE_PLAYER_SAFE",
                {
                    false,
                    1018,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_STAYS_IN_CURRENT_LEVEL",
                {
                    false,
                    1019,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_STAYS_IN_CURRENT_LEVEL",
                {
                    false,
                    1020,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_DRUNK_INPUT_DELAY",
                {
                    true,
                    1021,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_MONEY",
                {
                    true,
                    1022,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "INCREASE_CHAR_MONEY",
                {
                    false,
                    1023,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_OFFSET_FROM_OBJECT_IN_WORLD_COORDS",
                {
                    true,
                    1024,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_LIFE_SAVED",
                {
                    true,
                    1025,
                    {
                    }
                }
            },
            {
                "REGISTER_CRIMINAL_CAUGHT",
                {
                    true,
                    1026,
                    {
                    }
                }
            },
            {
                "REGISTER_AMBULANCE_LEVEL",
                {
                    true,
                    1027,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_FIRE_EXTINGUISHED",
                {
                    true,
                    1028,
                    {
                    }
                }
            },
            {
                "TURN_PHONE_ON",
                {
                    true,
                    1029,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_LONGEST_DODO_FLIGHT",
                {
                    false,
                    1030,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_OFFSET_FROM_CAR_IN_WORLD_COORDS",
                {
                    true,
                    1031,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_TOTAL_NUMBER_OF_KILL_FRENZIES",
                {
                    true,
                    1032,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "BLOW_UP_RC_BUGGY",
                {
                    true,
                    1033,
                    {
                    }
                }
            },
            {
                "REMOVE_CAR_FROM_CHASE",
                {
                    false,
                    1034,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_FRENCH_GAME",
                {
                    true,
                    1035,
                    {
                    }
                }
            },
            {
                "IS_GERMAN_GAME",
                {
                    true,
                    1036,
                    {
                    }
                }
            },
            {
                "CLEAR_MISSION_AUDIO",
                {
                    true,
                    1037,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_FADE_IN_AFTER_NEXT_ARREST",
                {
                    false,
                    1038,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_FADE_IN_AFTER_NEXT_DEATH",
                {
                    false,
                    1039,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_GANG_PED_MODEL_PREFERENCE",
                {
                    false,
                    1040,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_USE_PEDNODE_SEEK",
                {
                    true,
                    1041,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_VEHICLE_WEAPONS",
                {
                    false,
                    1042,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_GET_OUT_OF_JAIL_FREE",
                {
                    false,
                    1043,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_FREE_HEALTH_CARE",
                {
                    true,
                    1044,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_DOOR_CLOSED",
                {
                    false,
                    1045,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOAD_AND_LAUNCH_MISSION",
                {
                    true,
                    1047,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_OBJECT_DRAW_LAST",
                {
                    true,
                    1048,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_AMMO_IN_PLAYER_WEAPON",
                {
                    true,
                    1049,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_AMMO_IN_CHAR_WEAPON",
                {
                    false,
                    1050,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_KILL_FRENZY_PASSED",
                {
                    false,
                    1051,
                    {
                    }
                }
            },
            {
                "SET_CHAR_SAY",
                {
                    false,
                    1052,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_NEAR_CLIP",
                {
                    true,
                    1053,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_RADIO_CHANNEL",
                {
                    true,
                    1054,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "OVERRIDE_HOSPITAL_LEVEL",
                {
                    false,
                    1055,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "OVERRIDE_POLICE_STATION_LEVEL",
                {
                    false,
                    1056,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FORCE_RAIN",
                {
                    false,
                    1057,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DOES_GARAGE_CONTAIN_CAR",
                {
                    false,
                    1058,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_TRACTION",
                {
                    true,
                    1059,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "ARE_MEASUREMENTS_IN_METRES",
                {
                    true,
                    1060,
                    {
                    }
                }
            },
            {
                "CONVERT_METRES_TO_FEET",
                {
                    true,
                    1061,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "MARK_ROADS_BETWEEN_LEVELS",
                {
                    false,
                    1062,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "MARK_PED_ROADS_BETWEEN_LEVELS",
                {
                    false,
                    1063,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_AVOID_LEVEL_TRANSITIONS",
                {
                    true,
                    1064,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_AVOID_LEVEL_TRANSITIONS",
                {
                    false,
                    1065,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_THREAT_FOR_PED_TYPE",
                {
                    false,
                    1066,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_AREA_OF_CHARS",
                {
                    true,
                    1067,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TOTAL_NUMBER_OF_MISSIONS",
                {
                    true,
                    1068,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CONVERT_METRES_TO_FEET_INT",
                {
                    true,
                    1069,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_FASTEST_TIME",
                {
                    true,
                    1070,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_HIGHEST_SCORE",
                {
                    true,
                    1071,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "WARP_CHAR_INTO_CAR_AS_PASSENGER",
                {
                    false,
                    1072,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_PASSENGER_SEAT_FREE",
                {
                    true,
                    1073,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_CHAR_IN_CAR_PASSENGER_SEAT",
                {
                    false,
                    1074,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_IS_CHRIS_CRIMINAL",
                {
                    true,
                    1075,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "START_CREDITS",
                {
                    true,
                    1076,
                    {
                    }
                }
            },
            {
                "STOP_CREDITS",
                {
                    true,
                    1077,
                    {
                    }
                }
            },
            {
                "ARE_CREDITS_FINISHED",
                {
                    true,
                    1078,
                    {
                    }
                }
            },
            {
                "CREATE_SINGLE_PARTICLE",
                {
                    true,
                    1079,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_IGNORE_LEVEL_TRANSITIONS",
                {
                    false,
                    1080,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_CHASE_CAR",
                {
                    false,
                    1081,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "START_BOAT_FOAM_ANIMATION",
                {
                    false,
                    1082,
                    {
                    }
                }
            },
            {
                "UPDATE_BOAT_FOAM_ANIMATION",
                {
                    false,
                    1083,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_MUSIC_DOES_FADE",
                {
                    true,
                    1084,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_INTRO_IS_PLAYING",
                {
                    false,
                    1085,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_HOOKER",
                {
                    true,
                    1086,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PLAY_END_OF_GAME_TUNE",
                {
                    true,
                    1087,
                    {
                    }
                }
            },
            {
                "STOP_END_OF_GAME_TUNE",
                {
                    true,
                    1088,
                    {
                    }
                }
            },
            {
                "GET_CAR_MODEL",
                {
                    true,
                    1089,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_SITTING_IN_CAR",
                {
                    true,
                    1090,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_SITTING_IN_ANY_CAR",
                {
                    true,
                    1091,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_SCRIPT_FIRE_AUDIO",
                {
                    false,
                    1092,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ARE_ANY_CAR_CHEATS_ACTIVATED",
                {
                    true,
                    1093,
                    {
                    }
                }
            },
            {
                "SET_CHAR_SUFFERS_CRITICAL_HITS",
                {
                    true,
                    1094,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_LIFTING_A_PHONE",
                {
                    false,
                    1095,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_SITTING_IN_CAR",
                {
                    true,
                    1096,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_SITTING_IN_ANY_CAR",
                {
                    true,
                    1097,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_ON_FOOT",
                {
                    true,
                    1098,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_ON_FOOT",
                {
                    true,
                    1099,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOAD_COLLISION_WITH_SCREEN",
                {
                    false,
                    1100,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOAD_SPLASH_SCREEN",
                {
                    true,
                    1101,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_CAR_IGNORE_LEVEL_TRANSITIONS",
                {
                    false,
                    1102,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "MAKE_CRAIGS_CAR_A_BIT_STRONGER",
                {
                    false,
                    1103,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_JAMES_CAR_ON_PATH_TO_PLAYER",
                {
                    true,
                    1104,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOAD_END_OF_GAME_TUNE",
                {
                    true,
                    1105,
                    {
                    }
                }
            },
            {
                "ENABLE_PLAYER_CONTROL_CAMERA",
                {
                    false,
                    1106,
                    {
                    }
                }
            },
            {
                "SET_OBJECT_ROTATION",
                {
                    true,
                    1107,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_DEBUG_CAMERA_COORDINATES",
                {
                    true,
                    1108,
                    {
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_DEBUG_CAMERA_FRONT_VECTOR",
                {
                    false,
                    1109,
                    {
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_TARGETTING_ANY_CHAR",
                {
                    false,
                    1110,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_TARGETTING_CHAR",
                {
                    true,
                    1111,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_TARGETTING_OBJECT",
                {
                    false,
                    1112,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME",
                {
                    true,
                    1113,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "DISPLAY_TEXT_WITH_NUMBER",
                {
                    true,
                    1114,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DISPLAY_TEXT_WITH_2_NUMBERS",
                {
                    true,
                    1115,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FAIL_CURRENT_MISSION",
                {
                    true,
                    1116,
                    {
                    }
                }
            },
            {
                "GET_CLOSEST_OBJECT_OF_TYPE",
                {
                    false,
                    1117,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "PLACE_OBJECT_RELATIVE_TO_OBJECT",
                {
                    false,
                    1118,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_ALL_OCCUPANTS_OF_CAR_LEAVE_CAR",
                {
                    true,
                    1119,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_INTERPOLATION_PARAMETERS",
                {
                    true,
                    1120,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_CLOSEST_CAR_NODE_WITH_HEADING_TOWARDS_POINT",
                {
                    false,
                    1121,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_CLOSEST_CAR_NODE_WITH_HEADING_AWAY_POINT",
                {
                    false,
                    1122,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_DEBUG_CAMERA_POINT_AT",
                {
                    true,
                    1123,
                    {
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "ATTACH_CHAR_TO_CAR",
                {
                    true,
                    1124,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DETACH_CHAR_FROM_CAR",
                {
                    true,
                    1125,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_CHANGE_LANE",
                {
                    true,
                    1126,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_CHAR_LAST_WEAPON_DAMAGE",
                {
                    true,
                    1127,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_CAR_LAST_WEAPON_DAMAGE",
                {
                    true,
                    1128,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_RANDOM_COP_IN_AREA",
                {
                    true,
                    1129,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_RANDOM_COP_IN_ZONE",
                {
                    false,
                    1130,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_FLEE_CAR",
                {
                    true,
                    1131,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_DRIVER_OF_CAR",
                {
                    true,
                    1132,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_NUMBER_OF_FOLLOWERS",
                {
                    true,
                    1133,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "GIVE_REMOTE_CONTROLLED_MODEL_TO_PLAYER",
                {
                    true,
                    1134,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_CURRENT_PLAYER_WEAPON",
                {
                    true,
                    1135,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_CURRENT_CHAR_WEAPON",
                {
                    true,
                    1136,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_ANY_MEANS_OBJECT_2D",
                {
                    true,
                    1137,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_ON_FOOT_OBJECT_2D",
                {
                    true,
                    1138,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_IN_CAR_OBJECT_2D",
                {
                    false,
                    1139,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_ANY_MEANS_OBJECT_3D",
                {
                    false,
                    1140,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_ON_FOOT_OBJECT_3D",
                {
                    false,
                    1141,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_CHAR_IN_CAR_OBJECT_3D",
                {
                    false,
                    1142,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_TEMP_ACTION",
                {
                    true,
                    1143,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_HANDBRAKE_TURN_RIGHT",
                {
                    false,
                    1144,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_HANDBRAKE_STOP",
                {
                    false,
                    1145,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_ON_ANY_BIKE",
                {
                    true,
                    1146,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_SNIPER_BULLET_2D",
                {
                    false,
                    1147,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_SNIPER_BULLET_3D",
                {
                    false,
                    1148,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_NUMBER_OF_SEATS_IN_MODEL",
                {
                    false,
                    1149,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_ON_ANY_BIKE",
                {
                    true,
                    1150,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_LYING_DOWN",
                {
                    false,
                    1151,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CAN_CHAR_SEE_DEAD_CHAR",
                {
                    true,
                    1152,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_ENTER_CAR_RANGE_MULTIPLIER",
                {
                    true,
                    1153,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_THREAT_REACTION_RANGE_MULTIPLIER",
                {
                    true,
                    1154,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_CEASE_ATTACK_TIMER",
                {
                    true,
                    1155,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_REMOTE_CONTROLLED_CAR",
                {
                    true,
                    1156,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_PC_VERSION",
                {
                    true,
                    1157,
                    {
                    }
                }
            },
            {
                "REPLAY",
                {
                    false,
                    1158,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_REPLAY_PLAYING",
                {
                    false,
                    1159,
                    {
                    }
                }
            },
            {
                "IS_MODEL_AVAILABLE",
                {
                    true,
                    1160,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SHUT_CHAR_UP",
                {
                    true,
                    1161,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_ENABLE_RC_DETONATE",
                {
                    true,
                    1162,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_RANDOM_ROUTE_SEED",
                {
                    true,
                    1163,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_ANY_PICKUP_AT_COORDS",
                {
                    true,
                    1164,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_FIRST_PICKUP_COORDS",
                {
                    false,
                    1165,
                    {
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_NEXT_PICKUP_COORDS",
                {
                    false,
                    1166,
                    {
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "REMOVE_ALL_CHAR_WEAPONS",
                {
                    true,
                    1167,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_PLAYER_GOT_WEAPON",
                {
                    true,
                    1168,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_CHAR_GOT_WEAPON",
                {
                    false,
                    1169,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_FACING_CHAR",
                {
                    false,
                    1170,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TANK_DETONATE_CARS",
                {
                    true,
                    1171,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_POSITION_OF_ANALOGUE_STICKS",
                {
                    true,
                    1172,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_ON_FIRE",
                {
                    true,
                    1173,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_TYRE_BURST",
                {
                    true,
                    1174,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_DRIVE_STRAIGHT_AHEAD",
                {
                    false,
                    1175,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_WAIT",
                {
                    false,
                    1176,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_STANDING_ON_A_VEHICLE",
                {
                    false,
                    1177,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_FOOT_DOWN",
                {
                    false,
                    1178,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_FOOT_DOWN",
                {
                    false,
                    1179,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "INITIALISE_OBJECT_PATH",
                {
                    true,
                    1180,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "START_OBJECT_ON_PATH",
                {
                    true,
                    1181,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_OBJECT_PATH_SPEED",
                {
                    true,
                    1182,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_OBJECT_PATH_POSITION",
                {
                    true,
                    1183,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_OBJECT_DISTANCE_ALONG_PATH",
                {
                    false,
                    1184,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_OBJECT_PATH",
                {
                    true,
                    1185,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HELI_GOTO_COORDS",
                {
                    true,
                    1186,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_THING_EQUAL_TO_THING",
                {
                    true,
                    1187,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_THING_EQUAL_TO_THING",
                {
                    true,
                    1188,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_DEAD_CHAR_PICKUP_COORDS",
                {
                    true,
                    1189,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "CREATE_PROTECTION_PICKUP",
                {
                    true,
                    1190,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_ANY_BOAT",
                {
                    false,
                    1191,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_ANY_BOAT",
                {
                    true,
                    1192,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_ANY_HELI",
                {
                    false,
                    1193,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_ANY_HELI",
                {
                    true,
                    1194,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_ANY_PLANE",
                {
                    false,
                    1195,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_ANY_PLANE",
                {
                    true,
                    1196,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_WATER",
                {
                    true,
                    1197,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET",
                {
                    true,
                    1198,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET",
                {
                    true,
                    1199,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    1200,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    1201,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    1202,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_THAN_THING",
                {
                    true,
                    1203,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    1204,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    1205,
                    {
                        { ArgType::Integer, false, false, false, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    1206,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_THING_GREATER_OR_EQUAL_TO_THING",
                {
                    true,
                    1207,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, false, true,{} },
                    }
                }
            },
            {
                "GET_CHAR_WEAPON_IN_SLOT",
                {
                    true,
                    1208,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_CLOSEST_STRAIGHT_ROAD",
                {
                    true,
                    1209,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_FORWARD_SPEED",
                {
                    true,
                    1210,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_AREA_VISIBLE",
                {
                    true,
                    1211,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CUTSCENE_ANIM_TO_LOOP",
                {
                    true,
                    1212,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "MARK_CAR_AS_CONVOY_CAR",
                {
                    true,
                    1213,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "RESET_HAVOC_CAUSED_BY_PLAYER",
                {
                    true,
                    1214,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_HAVOC_CAUSED_BY_PLAYER",
                {
                    true,
                    1215,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "CREATE_SCRIPT_ROADBLOCK",
                {
                    true,
                    1216,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_ALL_SCRIPT_ROADBLOCKS",
                {
                    true,
                    1217,
                    {
                    }
                }
            },
            {
                "SET_CHAR_OBJ_WALK_TO_CHAR",
                {
                    true,
                    1218,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PICKUP_IN_ZONE",
                {
                    false,
                    1219,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "GET_OFFSET_FROM_CHAR_IN_WORLD_COORDS",
                {
                    true,
                    1220,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "HAS_CHAR_BEEN_PHOTOGRAPHED",
                {
                    true,
                    1221,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_AIM_GUN_AT_CHAR",
                {
                    true,
                    1222,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_SECURITY_CAMERA",
                {
                    true,
                    1223,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_FLYING_VEHICLE",
                {
                    false,
                    1224,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_FLYING_VEHICLE",
                {
                    true,
                    1225,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_SONY_CD_BEEN_READ",
                {
                    false,
                    1226,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_NUMBER_OF_SONY_CDS_READ",
                {
                    false,
                    1227,
                    {
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_SHORT_RANGE_BLIP_FOR_COORD_OLD",
                {
                    false,
                    1228,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_SHORT_RANGE_BLIP_FOR_COORD",
                {
                    false,
                    1229,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_SHORT_RANGE_SPRITE_BLIP_FOR_COORD",
                {
                    true,
                    1230,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_MONEY_SPENT_ON_CLOTHES",
                {
                    true,
                    1231,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_HELI_ORIENTATION",
                {
                    true,
                    1232,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_HELI_ORIENTATION",
                {
                    true,
                    1233,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PLANE_GOTO_COORDS",
                {
                    true,
                    1234,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_NTH_CLOSEST_CAR_NODE",
                {
                    true,
                    1235,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_NTH_CLOSEST_CHAR_NODE",
                {
                    false,
                    1236,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "DRAW_WEAPONSHOP_CORONA",
                {
                    true,
                    1237,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_ENABLE_RC_DETONATE_ON_CONTACT",
                {
                    true,
                    1238,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FREEZE_CHAR_POSITION",
                {
                    true,
                    1239,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_DROWNS_IN_WATER",
                {
                    true,
                    1240,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_OBJECT_RECORDS_COLLISIONS",
                {
                    true,
                    1241,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_OBJECT_COLLIDED_WITH_ANYTHING",
                {
                    true,
                    1242,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REMOVE_RC_BUGGY",
                {
                    true,
                    1243,
                    {
                    }
                }
            },
            {
                "HAS_PHOTOGRAPH_BEEN_TAKEN",
                {
                    false,
                    1244,
                    {
                    }
                }
            },
            {
                "GET_CHAR_ARMOUR",
                {
                    true,
                    1245,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_ARMOUR",
                {
                    false,
                    1246,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_HELI_STABILISER",
                {
                    true,
                    1247,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_STRAIGHT_LINE_DISTANCE",
                {
                    true,
                    1248,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "POP_CAR_BOOT",
                {
                    true,
                    1249,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SHUT_PLAYER_UP",
                {
                    true,
                    1250,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_MOOD",
                {
                    true,
                    1251,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REQUEST_COLLISION",
                {
                    true,
                    1252,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_OBJECT_2D",
                {
                    true,
                    1253,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOCATE_OBJECT_3D",
                {
                    true,
                    1254,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_OBJECT_IN_WATER",
                {
                    true,
                    1255,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_STEAL_ANY_CAR_EVEN_MISSION_CAR",
                {
                    false,
                    1256,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_OBJECT_IN_AREA_2D",
                {
                    false,
                    1257,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_OBJECT_IN_AREA_3D",
                {
                    true,
                    1258,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_CROUCH",
                {
                    true,
                    1259,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_ZONE_CIVILIAN_CAR_INFO",
                {
                    true,
                    1260,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REQUEST_ANIMATION",
                {
                    true,
                    1261,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "HAS_ANIMATION_LOADED",
                {
                    true,
                    1262,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "REMOVE_ANIMATION",
                {
                    true,
                    1263,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_CHAR_WAITING_FOR_WORLD_COLLISION",
                {
                    true,
                    1264,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_WAITING_FOR_WORLD_COLLISION",
                {
                    true,
                    1265,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_OBJECT_WAITING_FOR_WORLD_COLLISION",
                {
                    false,
                    1266,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_SHUFFLE_INTO_DRIVERS_SEAT",
                {
                    true,
                    1267,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ATTACH_CHAR_TO_OBJECT",
                {
                    true,
                    1268,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_AS_PLAYER_FRIEND",
                {
                    true,
                    1269,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DISPLAY_NTH_ONSCREEN_COUNTER",
                {
                    false,
                    1270,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DISPLAY_NTH_ONSCREEN_COUNTER_WITH_STRING",
                {
                    true,
                    1271,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "ADD_SET_PIECE",
                {
                    true,
                    1272,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_EXTRA_COLOURS",
                {
                    true,
                    1273,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_EXTRA_COLOURS",
                {
                    true,
                    1274,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLOSE_CAR_BOOT",
                {
                    false,
                    1275,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_WHEELIE_STATS",
                {
                    true,
                    1276,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "DISARM_CHAR",
                {
                    false,
                    1277,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "BURST_CAR_TYRE",
                {
                    true,
                    1278,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_OBJ_NO_OBJ",
                {
                    true,
                    1279,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_WEARING",
                {
                    true,
                    1280,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_PLAYER_CAN_DO_DRIVE_BY",
                {
                    true,
                    1281,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_SPRINT_TO_COORD",
                {
                    true,
                    1282,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "CREATE_SWAT_ROPE",
                {
                    true,
                    1283,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_FIRST_PERSON_CONTROL_CAMERA",
                {
                    false,
                    1284,
                    {
                    }
                }
            },
            {
                "GET_NEAREST_TYRE_TO_POINT",
                {
                    false,
                    1285,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CAR_MODEL_COMPONENTS",
                {
                    true,
                    1286,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SWITCH_LIFT_CAMERA",
                {
                    true,
                    1287,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLOSE_ALL_CAR_DOORS",
                {
                    true,
                    1288,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_DISTANCE_BETWEEN_COORDS_2D",
                {
                    true,
                    1289,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_DISTANCE_BETWEEN_COORDS_3D",
                {
                    true,
                    1290,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "POP_CAR_BOOT_USING_PHYSICS",
                {
                    true,
                    1291,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_FIRST_PERSON_WEAPON_CAMERA",
                {
                    false,
                    1292,
                    {
                    }
                }
            },
            {
                "IS_CHAR_LEAVING_VEHICLE_TO_DIE",
                {
                    true,
                    1293,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SORT_OUT_OBJECT_COLLISION_WITH_CAR",
                {
                    true,
                    1294,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_MAX_WANTED_LEVEL",
                {
                    false,
                    1295,
                    {
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_WANDER_PATH_CLEAR",
                {
                    true,
                    1296,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_HELP_WITH_NUMBER",
                {
                    false,
                    1297,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PRINT_HELP_FOREVER",
                {
                    true,
                    1298,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "PRINT_HELP_FOREVER_WITH_NUMBER",
                {
                    false,
                    1299,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_CAN_BE_DAMAGED_BY_MEMBERS_OF_GANG",
                {
                    true,
                    1300,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_MISSION_AUDIO_PLAYING",
                {
                    false,
                    1302,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CREATE_LOCKED_PROPERTY_PICKUP",
                {
                    true,
                    1303,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "CREATE_FORSALE_PROPERTY_PICKUP",
                {
                    true,
                    1304,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "FREEZE_CAR_POSITION",
                {
                    true,
                    1305,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_CHAR_BEEN_DAMAGED_BY_CHAR",
                {
                    true,
                    1306,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_CHAR_BEEN_DAMAGED_BY_CAR",
                {
                    false,
                    1307,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_CAR_BEEN_DAMAGED_BY_CHAR",
                {
                    false,
                    1308,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_CAR_BEEN_DAMAGED_BY_CAR",
                {
                    false,
                    1309,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_RADIO_CHANNEL",
                {
                    false,
                    1310,
                    {
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "DISPLAY_TEXT_WITH_3_NUMBERS",
                {
                    false,
                    1311,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_DROWNING_IN_WATER",
                {
                    false,
                    1312,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_DROWNING_IN_WATER",
                {
                    true,
                    1313,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DISABLE_CUTSCENE_SHADOWS",
                {
                    true,
                    1314,
                    {
                    }
                }
            },
            {
                "HAS_GLASS_BEEN_SHATTERED_NEARBY",
                {
                    true,
                    1315,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "ATTACH_CUTSCENE_OBJECT_TO_BONE",
                {
                    true,
                    1316,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ATTACH_CUTSCENE_OBJECT_TO_COMPONENT",
                {
                    true,
                    1317,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_CHAR_STAY_IN_CAR_WHEN_JACKED",
                {
                    true,
                    1318,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_MISSION_AUDIO_LOADING",
                {
                    false,
                    1319,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_MONEY_SPENT_ON_WEAPONS",
                {
                    true,
                    1320,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_MONEY_SPENT_ON_PROPERTY",
                {
                    true,
                    1321,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_MONEY_SPENT_ON_AUTO_PAINTING",
                {
                    false,
                    1322,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_ANSWERING_MOBILE",
                {
                    true,
                    1323,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_DRUNKENNESS",
                {
                    true,
                    1324,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_PLAYER_DRUNKENNESS",
                {
                    false,
                    1325,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_DRUG_LEVEL",
                {
                    false,
                    1326,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_PLAYER_DRUG_LEVEL",
                {
                    false,
                    1327,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_LOAN_SHARK_VISITS",
                {
                    false,
                    1328,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_STORES_KNOCKED_OFF",
                {
                    true,
                    1329,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_MOVIE_STUNTS",
                {
                    false,
                    1330,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_NUMBER_OF_ASSASSINATIONS",
                {
                    true,
                    1331,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_PIZZAS_DELIVERED",
                {
                    true,
                    1332,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_GARBAGE_PICKUPS",
                {
                    false,
                    1333,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_ICE_CREAMS_SOLD",
                {
                    true,
                    1334,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_TOP_SHOOTING_RANGE_SCORE",
                {
                    false,
                    1335,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_SHOOTING_RANGE_RANK",
                {
                    false,
                    1336,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_MONEY_SPENT_ON_GAMBLING",
                {
                    false,
                    1337,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_MONEY_WON_ON_GAMBLING",
                {
                    false,
                    1338,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_LARGEST_GAMBLING_WIN",
                {
                    false,
                    1339,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_IN_PLAYERS_GROUP_CAN_FIGHT",
                {
                    true,
                    1340,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_CHAR_WAIT_STATE",
                {
                    true,
                    1341,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_RANDOM_CAR_OF_TYPE_IN_AREA_NO_SAVE",
                {
                    true,
                    1342,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CAN_BURST_CAR_TYRES",
                {
                    true,
                    1343,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_AUTO_AIM",
                {
                    true,
                    1344,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FIRE_HUNTER_GUN",
                {
                    true,
                    1345,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PROPERTY_AS_OWNED",
                {
                    true,
                    1346,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_BLOOD_RING_KILLS",
                {
                    true,
                    1347,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_LONGEST_TIME_IN_BLOOD_RING",
                {
                    true,
                    1348,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REMOVE_EVERYTHING_FOR_HUGE_CUTSCENE",
                {
                    true,
                    1349,
                    {
                    }
                }
            },
            {
                "IS_PLAYER_TOUCHING_VEHICLE",
                {
                    true,
                    1350,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_TOUCHING_VEHICLE",
                {
                    false,
                    1351,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CHECK_FOR_PED_MODEL_AROUND_PLAYER",
                {
                    true,
                    1352,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_CHAR_FOLLOW_PATH",
                {
                    true,
                    1353,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_CAN_BE_SHOT_IN_VEHICLE",
                {
                    true,
                    1354,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ATTACH_CUTSCENE_OBJECT_TO_VEHICLE",
                {
                    true,
                    1355,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOAD_MISSION_TEXT",
                {
                    true,
                    1356,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "SET_TONIGHTS_EVENT",
                {
                    true,
                    1357,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_CHAR_LAST_DAMAGE_ENTITY",
                {
                    true,
                    1358,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_CAR_LAST_DAMAGE_ENTITY",
                {
                    false,
                    1359,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FREEZE_OBJECT_POSITION",
                {
                    true,
                    1360,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_HAS_MET_DEBBIE_HARRY",
                {
                    true,
                    1361,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_RIOT_INTENSITY",
                {
                    true,
                    1362,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_IN_ANGLED_AREA_2D",
                {
                    false,
                    1363,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CAR_IN_ANGLED_AREA_3D",
                {
                    false,
                    1364,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REMOVE_WEAPON_FROM_CHAR",
                {
                    false,
                    1365,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_UP_TAXI_SHORTCUT",
                {
                    true,
                    1366,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_TAXI_SHORTCUT",
                {
                    true,
                    1367,
                    {
                    }
                }
            },
            {
                "SET_CHAR_OBJ_GOTO_CAR_ON_FOOT",
                {
                    false,
                    1368,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_CLOSEST_WATER_NODE",
                {
                    false,
                    1369,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_PORN_LEAFLET_TO_RUBBISH",
                {
                    true,
                    1370,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CREATE_CLOTHES_PICKUP",
                {
                    true,
                    1371,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "CHANGE_BLIP_THRESHOLD",
                {
                    false,
                    1372,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "MAKE_PLAYER_FIRE_PROOF",
                {
                    true,
                    1373,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "INCREASE_PLAYER_MAX_HEALTH",
                {
                    true,
                    1374,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "INCREASE_PLAYER_MAX_ARMOUR",
                {
                    true,
                    1375,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CREATE_RANDOM_CHAR_AS_DRIVER",
                {
                    true,
                    1376,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "CREATE_RANDOM_CHAR_AS_PASSENGER",
                {
                    true,
                    1377,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_IGNORE_THREATS_BEHIND_OBJECTS",
                {
                    true,
                    1378,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ENSURE_PLAYER_HAS_DRIVE_BY_WEAPON",
                {
                    true,
                    1379,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "MAKE_HELI_COME_CRASHING_DOWN",
                {
                    true,
                    1380,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_EXPLOSION_NO_SOUND",
                {
                    true,
                    1381,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_OBJECT_AREA_VISIBLE",
                {
                    true,
                    1382,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "WAS_VEHICLE_EVER_POLICE",
                {
                    false,
                    1383,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_NEVER_TARGETTED",
                {
                    true,
                    1384,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "LOAD_UNCOMPRESSED_ANIM",
                {
                    true,
                    1385,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "WAS_CUTSCENE_SKIPPED",
                {
                    true,
                    1386,
                    {
                    }
                }
            },
            {
                "SET_CHAR_CROUCH_WHEN_THREATENED",
                {
                    true,
                    1387,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_IN_ANY_POLICE_VEHICLE",
                {
                    true,
                    1388,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DOES_CHAR_EXIST",
                {
                    true,
                    1389,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DOES_VEHICLE_EXIST",
                {
                    false,
                    1390,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_SHORT_RANGE_BLIP_FOR_CONTACT_POINT",
                {
                    false,
                    1391,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_SHORT_RANGE_SPRITE_BLIP_FOR_CONTACT_POINT",
                {
                    true,
                    1392,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_STUCK",
                {
                    true,
                    1393,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_ALL_TAXIS_HAVE_NITRO",
                {
                    true,
                    1394,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_STOP_SHOOT_DONT_SEEK_ENTITY",
                {
                    true,
                    1395,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FREEZE_CAR_POSITION_AND_DONT_LOAD_COLLISION",
                {
                    true,
                    1396,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FREEZE_CHAR_POSITION_AND_DONT_LOAD_COLLISION",
                {
                    false,
                    1397,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FREEZE_OBJECT_POSITION_AND_DONT_LOAD_COLLISION",
                {
                    false,
                    1398,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_FADE_AND_JUMPCUT_AFTER_RC_EXPLOSION",
                {
                    false,
                    1399,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_VIGILANTE_LEVEL",
                {
                    true,
                    1400,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CLEAR_ALL_CHAR_ANIMS",
                {
                    true,
                    1401,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_MAXIMUM_NUMBER_OF_CARS_IN_GARAGE",
                {
                    true,
                    1402,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "WANTED_STARS_ARE_FLASHING",
                {
                    true,
                    1403,
                    {
                    }
                }
            },
            {
                "SET_ALLOW_HURRICANES",
                {
                    true,
                    1404,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "PLAY_ANNOUNCEMENT",
                {
                    true,
                    1405,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_PLAYER_IS_IN_STADIUM",
                {
                    true,
                    1406,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_BUS_FARES_COLLECTED_BY_PLAYER",
                {
                    true,
                    1407,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_OBJ_BUY_ICE_CREAM",
                {
                    true,
                    1408,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "DISPLAY_RADAR",
                {
                    true,
                    1409,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_BEST_POSITION",
                {
                    true,
                    1410,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_PLAYER_IN_INFO_ZONE",
                {
                    true,
                    1411,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "CLEAR_CHAR_ICE_CREAM_PURCHASE",
                {
                    true,
                    1412,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_IN_CAR_FIRE_BUTTON_PRESSED",
                {
                    true,
                    1413,
                    {
                    }
                }
            },
            {
                "HAS_CHAR_ATTEMPTED_ATTRACTOR",
                {
                    true,
                    1414,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_LOAD_COLLISION_FOR_CAR_FLAG",
                {
                    true,
                    1415,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_LOAD_COLLISION_FOR_CHAR_FLAG",
                {
                    true,
                    1416,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_LOAD_COLLISION_FOR_OBJECT_FLAG",
                {
                    false,
                    1417,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "ADD_BIG_GUN_FLASH",
                {
                    true,
                    1418,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "HAS_CHAR_BOUGHT_ICE_CREAM",
                {
                    true,
                    1419,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_PROGRESS_PERCENTAGE",
                {
                    true,
                    1420,
                    {
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_SHORTCUT_PICKUP_POINT",
                {
                    true,
                    1421,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_SHORTCUT_DROPOFF_POINT_FOR_MISSION",
                {
                    true,
                    1422,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_RANDOM_ICE_CREAM_CUSTOMER_IN_AREA",
                {
                    true,
                    1423,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_RANDOM_ICE_CREAM_CUSTOMER_IN_ZONE",
                {
                    false,
                    1424,
                    {
                        { ArgType::TextLabel, false, true, false, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "UNLOCK_ALL_CAR_DOORS_IN_AREA",
                {
                    true,
                    1425,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_GANG_ATTACK_PLAYER_WITH_COPS",
                {
                    true,
                    1426,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_CHAR_FRIGHTENED_IN_JACKED_CAR",
                {
                    true,
                    1427,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_VEHICLE_TO_FADE_IN",
                {
                    true,
                    1428,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_ODDJOB_MISSION_PASSED",
                {
                    true,
                    1429,
                    {
                    }
                }
            },
            {
                "IS_PLAYER_IN_SHORTCUT_TAXI",
                {
                    true,
                    1430,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_CHAR_DUCKING",
                {
                    true,
                    1431,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "CREATE_DUST_EFFECT_FOR_CUTSCENE_HELI",
                {
                    true,
                    1432,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "REGISTER_FIRE_LEVEL",
                {
                    true,
                    1433,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_AUSTRALIAN_GAME",
                {
                    true,
                    1434,
                    {
                    }
                }
            },
            {
                "DISARM_CAR_BOMB",
                {
                    true,
                    1435,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_ONSCREEN_COUNTER_FLASH_WHEN_FIRST_DISPLAYED",
                {
                    true,
                    1436,
                    {
                        { ArgType::Integer, false, false, true, false,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "SHUFFLE_CARD_DECKS",
                {
                    true,
                    1437,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "FETCH_NEXT_CARD",
                {
                    true,
                    1438,
                    {
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "GET_OBJECT_VELOCITY",
                {
                    true,
                    1439,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_DEBUG_CAMERA_ON",
                {
                    true,
                    1440,
                    {
                    }
                }
            },
            {
                "ADD_TO_OBJECT_ROTATION_VELOCITY",
                {
                    true,
                    1441,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SET_OBJECT_ROTATION_VELOCITY",
                {
                    true,
                    1442,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_OBJECT_STATIC",
                {
                    true,
                    1443,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_ANGLE_BETWEEN_2D_VECTORS",
                {
                    true,
                    1444,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "DO_2D_RECTANGLES_COLLIDE",
                {
                    true,
                    1445,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_OBJECT_ROTATION_VELOCITY",
                {
                    true,
                    1446,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "ADD_VELOCITY_RELATIVE_TO_OBJECT_VELOCITY",
                {
                    true,
                    1447,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_OBJECT_SPEED",
                {
                    true,
                    1448,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_MISSION_SKIP",
                {
                    true,
                    1449,
                    {
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "SET_IN_AMMUNATION",
                {
                    true,
                    1450,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "DO_SAVE_GAME",
                {
                    true,
                    1451,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_RETRY",
                {
                    true,
                    1452,
                    {
                        { ArgType::Integer, false, false, true, true,{} },
                    }
                }
            },
            {
                "DUMMY",
                {
                    true,
                    1453,
                    {
                    }
                }
            },
            {
                "MARK_CUTSCENE_START",
                {
                    true,
                    1454,
                    {
                    }
                }
            },
            {
                "MARK_CUTSCENE_END",
                {
                    true,
                    1455,
                    {
                    }
                }
            },
            {
                "CUTSCENE_SCROLL",
                {
                    true,
                    1456,
                    {
                    }
                }
            },
            {
                "SET_LVAR_TEXT_LABEL_TO_LVAR_TEXT_LABEL",
                {
                    true,
                    1457,
                    {
                        { ArgType::TextLabel, false, false, false, true,{} },
                        { ArgType::TextLabel, false, false, false, true,{} },
                    }
                }
            },
            {
                "SET_VAR_TEXT_LABEL_TO_LVAR_TEXT_LABEL",
                {
                    true,
                    1458,
                    {
                        { ArgType::TextLabel, false, false, true, false,{} },
                        { ArgType::TextLabel, false, false, false, true,{} },
                    }
                }
            },
            {
                "SET_LVAR_TEXT_LABEL_TO_VAR_TEXT_LABEL",
                {
                    true,
                    1459,
                    {
                        { ArgType::TextLabel, false, false, false, true,{} },
                        { ArgType::TextLabel, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_TEXT_LABEL_VAR_EQUAL_TO_TEXT_LABEL",
                {
                    true,
                    1462,
                    {
                        { ArgType::TextLabel, false, false, true, false,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_TEXT_LABEL_LVAR_EQUAL_TO_TEXT_LABEL",
                {
                    true,
                    1463,
                    {
                        { ArgType::TextLabel, false, false, false, true,{} },
                        { ArgType::TextLabel, false, true, false, false,{} },
                    }
                }
            },
            {
                "IS_TEXT_LABEL_VAR_EQUAL_TO_TEXT_LABEL_VAR",
                {
                    true,
                    1464,
                    {
                        { ArgType::TextLabel, false, false, true, false,{} },
                        { ArgType::TextLabel, false, false, true, false,{} },
                    }
                }
            },
            {
                "IS_TEXT_LABEL_LVAR_EQUAL_TO_TEXT_LABEL_LVAR",
                {
                    true,
                    1465,
                    {
                        { ArgType::TextLabel, false, false, false, true,{} },
                        { ArgType::TextLabel, false, false, false, true,{} },
                    }
                }
            },
            {
                "IS_TEXT_LABEL_VAR_EQUAL_TO_TEXT_LABEL_LVAR",
                {
                    true,
                    1466,
                    {
                        { ArgType::TextLabel, false, false, true, false,{} },
                        { ArgType::TextLabel, false, false, false, true,{} },
                    }
                }
            },
            {
                "DO_2D_LINES_INTERSECT",
                {
                    true,
                    1467,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "GET_2D_LINES_INTERSECT_POINT",
                {
                    true,
                    1468,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                        { ArgType::Float, false, false, true, true,{} },
                    }
                }
            },
            {
                "IS_2D_POINT_IN_TRIANGLE",
                {
                    true,
                    1469,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_2D_POINT_IN_RECTANGLE_ON_LEFT_SIDE_OF_LINE",
                {
                    true,
                    1470,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "IS_2D_POINT_ON_LEFT_SIDE_OF_2D_LINE",
                {
                    true,
                    1471,
                    {
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "CHAR_LOOK_AT_OBJECT_ALWAYS",
                {
                    true,
                    1472,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Integer, false, true, true, true,{} },
                    }
                }
            },
            {
                "APPLY_COLLISION_ON_OBJECT",
                {
                    true,
                    1473,
                    {
                        { ArgType::Integer, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                        { ArgType::Float, false, true, true, true,{} },
                    }
                }
            },
            {
                "SAVE_STRING_TO_DEBUG_FILE",
                {
                    true,
                    1474,
                    {
                        { ArgType::Buffer128, false, true, false, false,{} },
                    }
                }
            },
        },

        // Enums
        {
            {
                "",  make_enum
                ({
                    // Boolean
                    { "FALSE", 0 },
                    { "TRUE",  1 },

                    // Boolean ON/OFF
                    { "OFF",    0 },
                    { "ON",     1 },

                    // Day/Night
                    { "NIGHT",  0 },
                    { "DAY",    1 },

                    // Kill Frenzy (yes, it's a free enum)
                    { "KILLFRENZY_INITIALLY",   0 },
                    { "KILLFRENZY_ONGOING",     1 },
                    { "KILLFRENZY_PASSED",      2 },
                    { "KILLFRENZY_FAILED",      3 },
                }),
            },
            { "FADE", fade_enum },
        },
    };
}
