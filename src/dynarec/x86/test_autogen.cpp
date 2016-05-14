/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.4.0, DynASM x86 version 1.4.0
** DO NOT EDIT! The original file is in "test.dasc".
*/

#line 1 "test.dasc"
#include "../../stdinc.h"
#include "../../disassembler.hpp"

#define DASM_EXTERN(ctx, addr, idx, type) resolve_extern(ctx, addr, idx, (type) != 0)

#include <dynasm/dasm_proto.h>

static int32_t resolve_extern(Dst_DECL, unsigned char* addr, unsigned int extern_idx, bool is_rel);
static int32_t resolve_extern(Dst_DECL, unsigned char* addr, const char* extern_name, bool is_rel);

#define resolve_extern_MACRO(a, b, c, d) resolve_extern(a, b, #c, d)

#include <dynasm/dasm_x86.h>

#define Dst &codegen.dstate

#define NotImplementedYet() __debugbreak()

//| .arch x86
#if DASM_VERSION != 10400
#error "Version mismatch between DynASM and included encoding engine"
#endif
#line 20 "test.dasc"
//| .actionlist actions
static const unsigned char actions[34] = {
  144,144,144,144,255,104,237,255,252,255,181,233,255,85,255,232,243,129,196,
  239,255,185,237,232,243,255,249,255,252,233,245,250,15,255
};

#line 21 "test.dasc"
//| .globals lbl_
enum {
  lbl__MAX
};
#line 22 "test.dasc"
//| .section code, imports
#define DASM_SECTION_CODE	0
#define DASM_SECTION_IMPORTS	1
#define DASM_MAXSECTION		2
#line 23 "test.dasc"
//| .externnames extern_table
static const char *const extern_table[] = {
  (const char *)0
};
#line 24 "test.dasc"

//| .macro xpushi32, varg
//| | codegen.emit_pushi32(varg);
//| .endmacro

//| .macro call_extern, target
//|| auto target_ptr_r8z9_..target = resolve_extern_MACRO(Dst, nullptr, target, false);
//|   .imports
//|       ->__imp__..target:
//|       .dword target_ptr_r8z9_..target
//|   .code
//|       call dword [->__imp__..target]
//| .endmacro

struct CodeGeneratorIA32;

//template<typename T, typename It>
//auto generate_code(const T&, CodeGeneratorIA32::IterData, CodeGeneratorIA32& codegen) -> CodeGeneratorIA32::IterData;
//auto generate_code(const DecompiledData& data, CodeGeneratorIA32::IterData, CodeGeneratorIA32& codegen) -> CodeGeneratorIA32::IterData;

template<typename T>
static optional<int32_t> get_imm32(const T&, CodeGeneratorIA32& codegen);
static optional<int32_t> get_imm32(const ArgVariant2&, CodeGeneratorIA32& codegen);

using DynarecError = CompilerError;

#define DynarecExpects Expects

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

struct tag_CRunningScript_t {};
constexpr tag_CRunningScript_t tag_CRunningScript;

// label 9: is used in emit_ccall

struct CodeGeneratorIA32
{
private:
    const Commands& commands;
    std::vector<DecompiledData> decompiled;
    char* global_vars;

public://TODO priv
    dasm_State *dstate;
    unsigned int dstate_maxpc;
    std::map<int32_t, unsigned int> pcmap;
    void* dlabels[lbl__MAX+1]; //+1 to avoid [0] sized array

    using IterData     = decltype(decompiled)::iterator;
    using opgen_func_t = std::function<IterData(const DecompiledCommand&, IterData)>;

    std::map<uint16_t, opgen_func_t> generators;

    friend auto generate_code(const DecompiledData& data, CodeGeneratorIA32::IterData, CodeGeneratorIA32& codegen)->CodeGeneratorIA32::IterData;

public:
    CodeGeneratorIA32(const Commands& commands, std::vector<DecompiledData> decompiled)
        : commands(commands), decompiled(std::move(decompiled))
    {
        dstate_maxpc = 0;
        dasm_init(&dstate, DASM_MAXSECTION);
        dasm_setupglobal(&dstate, dlabels, lbl__MAX);
        dasm_setup(&dstate, actions);

        global_vars = nullptr;

        this->init_generators();
    }

    CodeGeneratorIA32(const CodeGeneratorIA32&) = delete;

    CodeGeneratorIA32(CodeGeneratorIA32&&) = default;

    ~CodeGeneratorIA32()
    {
        dasm_free(&dstate);
    }

    void init_generators();

    void add_generator(int16_t opcode, opgen_func_t gen)
    {
        Expects((opcode & 0x8000) == 0);
        auto inpair = generators.emplace(opcode, std::move(gen));
        Ensures(inpair.second == true); // first time inserting
    }

    IterData run_generator(const DecompiledCommand& ccmd, IterData it)
    {
        auto genit = generators.find(ccmd.id & 0x7FFF);
        if(genit != generators.end())
            return (genit->second)(ccmd, std::move(it));
        throw DynarecError("Unsupported opcode {}", ccmd.id);
    }

    unsigned int add_label(int32_t label_param)
    {
        auto it = pcmap.find(label_param);
        if(it != pcmap.end())
            return it->second;

        ++dstate_maxpc;
        dasm_growpc(&this->dstate, dstate_maxpc);
        pcmap.emplace(label_param, dstate_maxpc - 1);
        return dstate_maxpc - 1;
    }

    unsigned int add_label(const ArgVariant2& varg)
    {
        if(auto opt_imm32 = get_imm32(varg, *this))
            return this->add_label(*opt_imm32);
        throw DynarecError("Unexpected ArgVariant2 on add_label; varg.which() == {}", varg.which());
    }


    void test()
    {
        auto& codegen = *this;

        for(auto it = decompiled.begin(); it != decompiled.end(); )
        {
            it = generate_code(*it, it, codegen);
        }

        //| nop
        //| nop
        //| nop
        //| nop
        dasm_put(Dst, 0);
#line 197 "test.dasc"

        size_t code_size;
        dasm_link(&dstate, &code_size);
        std::vector<uint8_t> buf(code_size, 0);
        dasm_encode(&dstate, buf.data());
        FILE* f = fopen("C:/Users/LINK/Desktop/__asm.bin", "wb");
        void* p = buf.data();
        fwrite(buf.data(), sizeof(uint8_t), buf.size(), f);
        fputc(0xB8, f);
        fwrite(&p, sizeof(p), 1, f);
        fclose(f);
        system("C:/Users/LINK/Desktop/__asm.bat");
        system("pause>nul");
        //__debugbreak();
    }

    void emit_flush()
    {
        // flush CRunningScript context/variables
    }

    void emit_pushi32(const ArgVariant2& varg)
    {
        auto& codegen = *this;

        if(auto opt_imm32 = get_imm32(varg, codegen))
        {
            //| push opt_imm32.value()
            dasm_put(Dst, 5, opt_imm32.value());
#line 225 "test.dasc"
        }
        else if(is<DecompiledVar>(varg))
        {
            auto& var = get<DecompiledVar>(varg);
            if(var.global)
            {
                //| push (global_vars + var.offset)
                dasm_put(Dst, 5, (global_vars + var.offset));
#line 232 "test.dasc"
            }
            else
            {
                auto offset = offsetof(CRunningScript, tls) + (var.offset * 4);
                //| push dword [ebp+offset]
                dasm_put(Dst, 8, offset);
#line 237 "test.dasc"
            }
        }
        else if(is<DecompiledVarArray>(varg))
        {
            // TODO
            NotImplementedYet();
        }
        else
        {
            throw DynarecError("Unexpected ArgVariant2 on emit_pushi32; varg.which() == {}", varg.which());
        }
    }

    void emit_push(tag_CRunningScript_t)
    {
        auto& codegen = *this;
        //| push ebp
        dasm_put(Dst, 13);
#line 254 "test.dasc"
    }

    void emit_push(const ArgVariant2& varg)
    {
        return emit_pushi32(varg);
    }

    template<typename... Args>
    void emit_ccall(const char* extern_name, Args&&... args)
    {
        auto& codegen = *this;
        auto target_ptr = resolve_extern(&this->dstate, nullptr, extern_name, false);
        this->emit_rpushes(std::forward<Args>(args)...);
        //| call &target_ptr
        //| add esp, (sizeof...(args) * 4)
        dasm_put(Dst, 15, (ptrdiff_t)(target_ptr), (sizeof...(args) * 4));
#line 269 "test.dasc"
    }

    template<typename... Args>
    void emit_thiscall(const char* extern_name, void* this_ptr, Args&&... args)
    {
        auto& codegen = *this;
        auto target_ptr = resolve_extern(&this->dstate, nullptr, extern_name, false);
        this->emit_rpushes(std::forward<Args>(args)...);
        //| mov ecx, this_ptr     // TODO abstract the mov
        //| call &target_ptr
        dasm_put(Dst, 21, this_ptr, (ptrdiff_t)(target_ptr));
#line 279 "test.dasc"
        // callee cleanup
    }

    template<typename... Args>
    void emit_stdcall(const char* extern_name, Args&&... args)
    {
        auto& codegen = *this;
        auto target_ptr = resolve_extern(&this->dstate, nullptr, extern_name, false);
        this->emit_rpushes(std::forward<Args>(args)...);
        //| call &target_ptr
        dasm_put(Dst, 23, (ptrdiff_t)(target_ptr));
#line 289 "test.dasc"
        // callee cleanup
    }

private:
    template<typename Arg, typename... Rest>
    void emit_rpushes(Arg&& arg, Rest&&... rest)
    {
        this->emit_rpushes(std::forward<Rest>(rest)...);
        this->emit_push(std::forward<Arg>(arg));
    }

    void emit_rpushes()
    {
    }
};

int32_t resolve_extern(dasm_State **dstate, unsigned char* addr, unsigned int eidx, bool is_rel)
{
    return resolve_extern(dstate, addr, extern_table[eidx], is_rel);
}

int32_t resolve_extern(dasm_State **dstate, unsigned char* addr, const char* extern_name, bool is_rel)
{
    if(!strcmp("DYNAREC_RTL_Wait", extern_name))
    {
        auto extern_ptr = (unsigned char*)0x7000;
        return is_rel? int32_t(extern_ptr - (addr + 4)) : int32_t(extern_ptr);
    }
    Unreachable();
}

auto generate_code(const DecompiledCommand& ccmd, CodeGeneratorIA32::IterData it, CodeGeneratorIA32& codegen) -> CodeGeneratorIA32::IterData
{
    return codegen.run_generator(ccmd, std::move(it));
}

auto generate_code(const DecompiledLabelDef& def, CodeGeneratorIA32::IterData it, CodeGeneratorIA32& codegen) -> CodeGeneratorIA32::IterData
{
    auto label_id = codegen.add_label(-def.offset);

    // flush context, the beggining of label should have all the context in CRunningScript
    codegen.emit_flush();
    //| =>(label_id):
    dasm_put(Dst, 26, (label_id));
#line 332 "test.dasc"

    return ++it;
}

auto generate_code(const DecompiledHex& hex, CodeGeneratorIA32::IterData it, CodeGeneratorIA32& codegen) -> CodeGeneratorIA32::IterData
{
    // TODO
    return ++it;
}

auto generate_code(const DecompiledData&, CodeGeneratorIA32::IterData it, CodeGeneratorIA32& codegen) -> CodeGeneratorIA32::IterData
{
    return visit_one(it->data, [&](const auto& data) { return ::generate_code(data, std::move(it), codegen); });
}






optional<int32_t> get_imm32(const EOAL&, CodeGeneratorIA32& codegen)
{
    return nullopt;
}

optional<int32_t> get_imm32(const DecompiledVar&, CodeGeneratorIA32& codegen)
{
    return nullopt;
}

optional<int32_t> get_imm32(const DecompiledVarArray&, CodeGeneratorIA32& codegen)
{
    return nullopt;
}

optional<int32_t> get_imm32(const DecompiledString&, CodeGeneratorIA32& codegen)
{
    return nullopt;
}

optional<int32_t> get_imm32(const int8_t& i8, CodeGeneratorIA32& codegen)
{
    return static_cast<int32_t>(i8);
}

optional<int32_t> get_imm32(const int16_t& i16, CodeGeneratorIA32& codegen)
{
    return static_cast<int32_t>(i16);
}

optional<int32_t> get_imm32(const int32_t& i32, CodeGeneratorIA32& codegen)
{
    return static_cast<int32_t>(i32);
}

optional<int32_t> get_imm32(const float& flt, CodeGeneratorIA32& codegen)
{
    // TODO floating point format static assert
    return reinterpret_cast<const int32_t&>(flt);
}

optional<int32_t> get_imm32(const ArgVariant2& varg, CodeGeneratorIA32& codegen)
{
    return visit_one(varg, [&](const auto& arg) { return ::get_imm32(arg, codegen); });
}





void CodeGeneratorIA32::init_generators()
{
    auto& codegen = *this;

    this->add_generator(0x0000, [&](const DecompiledCommand& ccmd, IterData it)
    {
        // TODO, this op should only appear on the top of a script!?!?
        return ++it;
    });

    this->add_generator(0x0001, [&](const DecompiledCommand& ccmd, IterData it)
    {
        Expects(ccmd.args.size() == 1);
        codegen.emit_ccall("DYNAREC_RTL_Wait", tag_CRunningScript, ccmd.args[0]);
        return ++it;
    });

    this->add_generator(0x0002, [&](const DecompiledCommand& ccmd, IterData it)
    {
        Expects(ccmd.args.size() == 1);
        auto label_id = codegen.add_label(ccmd.args[0]);

        // flush context before jumping into label
        codegen.emit_flush();
        //| jmp =>(label_id)
        //| .align 16 // nice place to put Intel's recommended alignment
        dasm_put(Dst, 28, (label_id));
#line 428 "test.dasc"

        return ++it;
    });



}








int test_dasc(const Commands& commands, std::vector<DecompiledData> decompiled)
{
    auto ia32 = CodeGeneratorIA32(commands, std::move(decompiled));
    ia32.test();
    return 0;
}

