#pragma once
#include <cstddef>

typedef union
{
    uint32_t	dwParam;
    int		nParam;
    float	fParam;
    void *	pParam;
    char *	szParam;
} SCRIPT_VAR;

#pragma pack(push,1)
struct CScriptThread
{
    CScriptThread	*next;					//next script in queue
    CScriptThread	*prev;					//previous script in queue
    char			threadName[8];			//name of thread, given by 03A4 opcode
    uint8_t			*baseIp;				//pointer to begin of script in memory
    uint8_t			*ip;					//current index pointer
    uint8_t			*stack[8];				//return stack for 0050, 0051
    uint16_t			sp;						//current item in stack
    uint16_t			_f3A;					//padding
    SCRIPT_VAR		tls[34];				//thread's local variables
    uint8_t			isActive;				//is current thread active
    char			condResult;				//condition result (true or false)
    char			missionCleanupFlag;		//clean mission
    char			external;				//is thread external (from script.img)
    uint8_t			_fC8;					//unknown
    uint8_t			_fC9;					//unknown
    uint8_t			_fCA;					//unknown
    uint8_t			_fCB;					//unknown
    uint32_t			wakeTime;				//time, when script starts again after 0001 opcode
    uint16_t			logicalOp;				//00D6 parameter
    uint8_t			notFlag;				//opcode & 0x8000 != 0
    uint8_t			wbCheckEnabled;			//wasted_or_busted check flag
    uint8_t			wastedOrBusted;			//is player wasted or busted
    uint8_t			_fD5;					//unknown
    uint16_t			_fD6;					//unknown
    uint32_t			sceneSkip;				//scene skip label ptr
    uint8_t			missionFlag;			//is mission thread
    uint8_t			_fDD[3];				//padding
};
#pragma pack(pop)

using CRunningScript = CScriptThread;
