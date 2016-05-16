#include "../../stdinc.h"
#include "codegen_ia32.hpp"

bool native(struct OpCtx& context);


CodeGeneratorIA32::CodeGeneratorIA32(const Commands& commands, std::vector<DecompiledData> decompiled) :
    commands(commands), decompiled(std::move(decompiled)),
    dstate{ *this, nullptr },
    // careful with the order
    regstates { {Reg::Eax}, {Reg::Ecx}, {Reg::Edx}, {Reg::Ebx}, {Reg::Esp}, {Reg::Ebp}, {Reg::Esi}, {Reg::Edi} }
{
    this->init_dasm();

    // Signal end of script
    decompiled.emplace_back(DecompiledLabelDef { SIZE_MAX });

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
        extern_ptr = (unsigned char*)0x7000;
    if(!strcmp("DYNAREC_RTL_WriteMemory", extern_name))
        extern_ptr = (unsigned char*)0x8000;
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
