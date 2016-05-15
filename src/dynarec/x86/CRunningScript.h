#include <cstddef>

using DWORD = uint32_t;
using WORD = uint16_t;
using BYTE = uint8_t;

typedef union
{
    DWORD	dwParam;
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
    BYTE			*baseIp;				//pointer to begin of script in memory
    BYTE			*ip;					//current index pointer
    BYTE			*stack[8];				//return stack for 0050, 0051
    WORD			sp;						//current item in stack
    WORD			_f3A;					//padding
    SCRIPT_VAR		tls[34];				//thread's local variables
    BYTE			isActive;				//is current thread active
    char			condResult;				//condition result (true or false)
    char			missionCleanupFlag;		//clean mission
    char			external;				//is thread external (from script.img)
    BYTE			_fC8;					//unknown
    BYTE			_fC9;					//unknown
    BYTE			_fCA;					//unknown
    BYTE			_fCB;					//unknown
    DWORD			wakeTime;				//time, when script starts again after 0001 opcode
    WORD			logicalOp;				//00D6 parameter
    BYTE			notFlag;				//opcode & 0x8000 != 0
    BYTE			wbCheckEnabled;			//wasted_or_busted check flag
    BYTE			wastedOrBusted;			//is player wasted or busted
    BYTE			_fD5;					//unknown
    WORD			_fD6;					//unknown
    DWORD			sceneSkip;				//scene skip label ptr
    BYTE			missionFlag;			//is mission thread
    BYTE			_fDD[3];				//padding
};
#pragma pack(pop)

using CRunningScript = CScriptThread;
