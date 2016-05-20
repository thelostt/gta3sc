#include "../../stdinc.h"
#include "codegen_ia32.hpp"

bool native(struct OpCtx& context);

#include "CRunningScript.h"

static void __fastcall DYNAREC_RTL_Wait(CRunningScript* script, int32_t time);
static void __fastcall DYNAREC_RTL_Resume(CRunningScript* script, struct DynarecScriptData* script_data);


CodeGeneratorIA32::CodeGeneratorIA32(const Commands& commands, std::vector<DecompiledData> decompiled) :
    commands(commands), decompiled(std::move(decompiled)),
    dstate{ *this, nullptr },
    // careful with the order
    regstates { {Reg::Eax}, {Reg::Ecx}, {Reg::Edx}, {Reg::Ebx}, {Reg::Esp}, {Reg::Ebp}, {Reg::Esi}, {Reg::Edi} }
{
    this->init_dasm();

    // Signal end of script
    this->decompiled.emplace_back(DecompiledLabelDef { SIZE_MAX });

    this->reg_ctx = this->regalloc(Reg::Ebp);
    this->reg_esp = this->regalloc(Reg::Esp);
    this->global_vars = nullptr;

    this->init_generators();
}

CodeGeneratorIA32::~CodeGeneratorIA32()
{
    this->free_dasm();
}

void CodeGeneratorIA32::add_generator(int16_t opcode, opgen_func_t gen)
{
    Expects((opcode & 0x8000) == 0);
    auto inpair = generators.emplace(opcode, std::move(gen));
    Ensures(inpair.second == true); // first time inserting
}

auto CodeGeneratorIA32::run_generator(const DecompiledCommand& ccmd, IterData it) -> IterData
{
    auto genit = generators.find(ccmd.id & 0x7FFF);
    if(genit != generators.end())
        return (genit->second)(ccmd, std::move(it));
    throw DynarecError("Unsupported opcode {}", ccmd.id);
}

int32_t CodeGeneratorIA32::resolve_extern(unsigned char* addr, const char* extern_name, bool is_rel)
{
    unsigned char* extern_ptr = nullptr;

    if(!strcmp("DYNAREC_RTL_Wait", extern_name))
        extern_ptr = (unsigned char*)&DYNAREC_RTL_Wait;//////////////////
    else if(!strcmp("DYNAREC_RTL_WriteMemory", extern_name))
        extern_ptr = (unsigned char*)0x8100;
    else if(!strcmp("DYNAREC_RTL_ReadMemory", extern_name))
        extern_ptr = (unsigned char*)0x8200;
    else if(!strcmp("DYNAREC_RTL_TerminateCs", extern_name))
        extern_ptr = (unsigned char*)0x8300;
    else if(!strcmp("native", extern_name))
        extern_ptr = (unsigned char*)&native;


    if(extern_ptr)
    {
        return is_rel? int32_t(extern_ptr - (addr + 4)) : int32_t(extern_ptr);
    }
    else
    {
        Unreachable();
    }
}










int test_dasc(const Commands& commands, std::vector<DecompiledData> decompiled)
{
    auto ia32 = CodeGeneratorIA32(commands, std::move(decompiled));
    ia32.test();
    return 0;
}


struct OpCtx{
    int pop() {return 0;}
    void store(int) {}
};


bool native2(int32_t pedtype, int32_t model, float x, float y, float z, int32_t* out_player)
{
    return true;
}

bool native(OpCtx& context)
{
    int32_t a = context.pop();
    int32_t b = context.pop();
    float x = context.pop();
    float y = context.pop();
    float z = context.pop();
    int32_t out;
    bool result = native2(a, b, x, y, z, &out);
    context.store(out);
    return result;
}

#include "CRunningScript.h"

struct DynarecScriptData
{
    uint8_t* old_baseip;  // the original base ip, allocated by CLEO

    // the new base ip
    std::unique_ptr<uint8_t> baseip;

    CRunningScript* script;

#ifndef NDEBUG
    uint32_t old_esp;
#endif
    uint32_t script_eip;
    uint32_t script_ebx;
    uint32_t script_ebp;
    uint32_t script_esi;
    uint32_t script_edi;
};


static char safety1[10000];
static std::map<CRunningScript*, DynarecScriptData> data_list;
static char safety2[10000];

extern "C"
bool DYNAREC_LoadScript(CRunningScript* script, const uint8_t* bytecode, size_t size)
{
    static uint8_t only[] = { 0x00, 0x00, 0x02, 0x00, 0x01, 0xf7, 0xff, 0xff, 0xff };

    if(size > sizeof(only)
        && memcmp(bytecode, only, sizeof(only)) != 0)
    {
        return false;
    }

    DynarecScriptData drc;

    std::vector<DecompiledData> decompiled;

    GameConfig config;
    config.use_half_float = false;
    config.has_text_label_prefix = true;

    Commands commands = gta3_commands();

    try
    {
        auto dcomp = Disassembler(std::move(config), commands, bytecode, size);
        dcomp.run_analyzer();
        decompiled = dcomp.get_data();
    }
    catch(const DecompilerError&)
    {
        __debugbreak();
        return false;
    }

    try
    {
        auto codegen = CodeGeneratorIA32(commands, std::move(decompiled));
        codegen.generate();
        drc.baseip = codegen.assemble().first;
    }
    catch(const DynarecError&)
    {
        __debugbreak();
        return false;
    }
    catch(const broken_contract&)
    {
        __debugbreak();
        return false;
    }

    drc.script_eip = reinterpret_cast<uint32_t>(drc.baseip.get());
    drc.script_ebp = reinterpret_cast<uint32_t>(script);

    drc.script     = script;
    drc.old_baseip = script->baseIp;
    script->baseIp = drc.baseip.get();

    data_list.emplace(script, std::move(drc));
    return true;
}

extern "C"
bool DYNAREC_FreeScript(CRunningScript* script)
{
    auto it = data_list.find(script);
    if(it != data_list.end())
    {
        auto& drc = it->second;
        
        Expects(drc.baseip.get() == script->baseIp);
        script->baseIp = drc.old_baseip;

        data_list.erase(it);
        return true;
    }
    return false;
}

extern "C"
bool DYNAREC_ProcessScript(CRunningScript* script)
{
    auto it = data_list.find(script);
    if(it != data_list.end())
    {
        DYNAREC_RTL_Resume(script, &it->second);
        return true;
    }
    return false;
}


static DynarecScriptData* current_script_data = nullptr;

void __declspec(naked) __fastcall DYNAREC_RTL_Wait(CRunningScript* script, int32_t time)
{
    _asm
    {
        // ecx=script, edx=time

        // Save wakeTime
        add edx, dword ptr [0xB7CB84] // CTimer::TimeInMilliseconds, TODO different address per game
        mov [ecx+CRunningScript::wakeTime], edx

        // ecx=script, edx=free

        // Pop eip to return to on the next script iteration.
        // TODO how bad is the performance hit of breaking the branch prediction with this?
        pop eax

        //
        mov edx, current_script_data

#ifndef NDEBUG
        cmp esp, [edx+DynarecScriptData::old_esp]
        jne _EspFuckedUp
#endif

        // Save script registers
        mov [edx+DynarecScriptData::script_eip], eax
        mov [edx+DynarecScriptData::script_ebx], ebx
        mov [edx+DynarecScriptData::script_ebp], ebp
        mov [edx+DynarecScriptData::script_esi], esi
        mov [edx+DynarecScriptData::script_edi], edi

        // Pop values pushed on DYNAREC_RTL_Resume
        pop edi
        pop esi
        pop ebp
        pop ebx

        // Return to whoever called DYNAREC_RTL_Resume
        ret

    _EspFuckedUp:
        ud2
    }
}

void __declspec(naked) __fastcall DYNAREC_RTL_Resume(CRunningScript* script, DynarecScriptData* script_data)
{
    _asm
    {
        // ecx=script
        // edx=script_data

        mov eax, dword ptr[0xB7CB84] // CTimer::TimeInMilliseconds, TODO different address per game
        cmp eax, [ecx+CRunningScript::wakeTime]
        jb _ScriptSleeping

        // Those should be poped again on DYNAREC_RTL_Wait
        push ebx
        push ebp
        push esi
        push edi

        // Restore script registers
        mov eax, [edx+DynarecScriptData::script_eip]
        mov ebx, [edx+DynarecScriptData::script_ebx]
        mov ebp, [edx+DynarecScriptData::script_ebp]
        mov esi, [edx+DynarecScriptData::script_esi]
        mov edi, [edx+DynarecScriptData::script_edi]

#ifndef NDEBUG
        mov [edx+DynarecScriptData::old_esp], esp
#endif

        // Jump back to where the jitted script stopped.
        mov current_script_data, edx
        jmp eax

    _ScriptSleeping:
        ret
    }
}
