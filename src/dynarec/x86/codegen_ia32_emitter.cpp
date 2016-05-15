/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.4.0, DynASM x86 version 1.4.0
** DO NOT EDIT! The original file is in "codegen_ia32_emitter.cpp.dasc".
*/

#line 1 "codegen_ia32_emitter.cpp.dasc"
#include "../../stdinc.h"
#include "codegen_ia32.hpp"
#include "CRunningScript.h"

#define Dst_DECL    CodeGeneratorIA32::Dst_DECL_t* Dst
#define Dst_REF	    (Dst->dasm_state)
#define DASM_FDEF   static
#include <dynasm/dasm_proto.h>
#include <dynasm/dasm_x86.h>

//| .arch x86
#if DASM_VERSION != 10400
#error "Version mismatch between DynASM and included encoding engine"
#endif
#line 12 "codegen_ia32_emitter.cpp.dasc"
//| .actionlist actions
static const unsigned char actions[98] = {
  144,144,144,144,255,249,255,252,233,245,250,15,255,232,243,255,104,237,255,
  80,240,32,255,252,255,53,237,255,252,255,181,233,255,184,240,32,237,255,139,
  5,240,129,237,255,139,133,253,240,129,233,255,137,5,240,129,237,255,137,133,
  253,240,129,233,255,199,5,237,237,255,199,133,233,237,255,129,5,253,237,239,
  255,129,133,233,239,255,1,5,240,129,237,255,1,133,253,240,129,233,255
};

#line 13 "codegen_ia32_emitter.cpp.dasc"
//| .globals lbl_
enum {
  lbl__MAX
};
#line 14 "codegen_ia32_emitter.cpp.dasc"
//| .section code
#define DASM_SECTION_CODE	0
#define DASM_MAXSECTION		1
#line 15 "codegen_ia32_emitter.cpp.dasc"
//| .externnames extern_table
static const char *const extern_table[] = {
  (const char *)0
};
#line 16 "codegen_ia32_emitter.cpp.dasc"

#define NotImplementedYet() (throw DynarecError("NotImplementedYet"))

static constexpr CodeGeneratorIA32::tag_CRunningScript_t tag_CRunningScript{};

void CodeGeneratorIA32::init_dasm()
{
    //static_assert(std::size(this->dlabels) >= lbl__MAX, "");
    static_assert(sizeof(this->dlabels) / sizeof(*this->dlabels) >= lbl__MAX, "");

    dstate_maxpc = 0;
    dasm_init(&dstate, DASM_MAXSECTION);
    dasm_setupglobal(&dstate, dlabels, lbl__MAX);
    dasm_setup(&dstate, actions);
}

void CodeGeneratorIA32::free_dasm()
{
    dasm_free(&dstate);
}

void CodeGeneratorIA32::test()
{
    auto& codegen = *this;

    for(auto it = decompiled.begin(); it != decompiled.end(); )
    {
        it = generate_code(*it, it, codegen);
    }

    /*
    //| nop
    //| nop
    //| nop
    //| nop
    dasm_put(Dst, 0);
#line 51 "codegen_ia32_emitter.cpp.dasc"
    */

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

static int32_t dasm_resolve_extern(CodeGeneratorIA32::Dst_DECL_t* dstate, unsigned char* addr, unsigned int eidx, bool is_rel)
{
    return dstate->codegen.resolve_extern(addr, extern_table[eidx], is_rel);
}


///////////////////////////////////////////////
// Labels
///////////////////////////////////////////////

unsigned int CodeGeneratorIA32::add_label(int32_t label_param)
{
    auto it = pcmap.find(label_param);
    if(it != pcmap.end())
        return it->second;

    ++dstate_maxpc;
    dasm_growpc(&this->dstate, dstate_maxpc);
    pcmap.emplace(label_param, dstate_maxpc - 1);
    return dstate_maxpc - 1;
}

unsigned int CodeGeneratorIA32::add_label(const ArgVariant2& varg)
{
    if(auto opt_imm32 = get_imm32(varg, *this))
        return this->add_label(*opt_imm32);
    throw DynarecError("Unexpected ArgVariant2 on add_label; varg.which() == {}", varg.which());
}

///////////////////////////////////////////////
// Overloads for generate_code
///////////////////////////////////////////////

#undef Dst
#define Dst &codegen.dstate

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
    dasm_put(Dst, 5, (label_id));
#line 116 "codegen_ia32_emitter.cpp.dasc"

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


///////////////////////////////
// Generators
///////////////////////////////

void CodeGeneratorIA32::init_generators()
{
    auto& codegen = *this;

    // NOP
    this->add_generator(0x0000, [&](const DecompiledCommand& ccmd, IterData it)
    {
        // TODO, this op should only appear on the top of a script!?!?
        return ++it;
    });

    // WAIT
    this->add_generator(0x0001, [&](const DecompiledCommand& ccmd, IterData it)
    {
        Expects(ccmd.args.size() == 1);
        codegen.emit_ccall("DYNAREC_RTL_Wait", tag_CRunningScript, ccmd.args[0]);
        return ++it;
    });

    // GOTO
    this->add_generator(0x0002, [&](const DecompiledCommand& ccmd, IterData it)
    {
        Expects(ccmd.args.size() == 1);
        auto label_id = codegen.add_label(ccmd.args[0]);

        // flush context before jumping into label
        codegen.emit_flush();
        //| jmp =>(label_id)
        //| .align 16 // nice place to put Intel's recommended alignment
        dasm_put(Dst, 7, (label_id));
#line 165 "codegen_ia32_emitter.cpp.dasc"

            return ++it;
    });

    // SET
    {
        auto opgen_set = [&](const DecompiledCommand& ccmd, IterData it)
        {
            Expects(ccmd.args.size() == 2);
            codegen.emit_movi32(ccmd.args[0], ccmd.args[1]);
            return ++it;
        };

        this->add_generator(0x0004, opgen_set);
        this->add_generator(0x0005, opgen_set);
        this->add_generator(0x0006, opgen_set);
        this->add_generator(0x0007, opgen_set);
        this->add_generator(0x0084, opgen_set);
        this->add_generator(0x0085, opgen_set);
        this->add_generator(0x0086, opgen_set);
        this->add_generator(0x0087, opgen_set);
        this->add_generator(0x0088, opgen_set);
        this->add_generator(0x0089, opgen_set);
        this->add_generator(0x008A, opgen_set);
        this->add_generator(0x008B, opgen_set);
        this->add_generator(0x04AE, opgen_set);
        this->add_generator(0x04AF, opgen_set);
    }

    // NOTICE: FLOATHING ADD IS DIFFERENT FROM INT ADD!
    this->add_generator(0x000A, [&](const DecompiledCommand& ccmd, IterData it)
    {
        Expects(ccmd.args.size() == 2);
        codegen.emit_addi32(ccmd.args[0], ccmd.args[1]);
        return ++it;
    });


}


///////////////////////////////////////////////
// Emitters
///////////////////////////////////////////////

#undef Dst
#define Dst &this->dstate

void CodeGeneratorIA32::emit_call(int32_t target_ptr)
{
    //| call &target_ptr
    dasm_put(Dst, 13, (ptrdiff_t)(target_ptr));
#line 216 "codegen_ia32_emitter.cpp.dasc"
}




/////////////////////
// PUSHI32

void CodeGeneratorIA32::emit_pushi32(int32_t imm32)
{
    //| push imm32
    dasm_put(Dst, 16, imm32);
#line 227 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_pushi32(RegGuard& reg)
{
    //| push Rd(reg->id)
    dasm_put(Dst, 19, (reg->id));
#line 232 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_pushi32(const DecompiledVar& var)
{
    if(var.global)
    {
        //| push dword[(global_vars + var.offset)]
        dasm_put(Dst, 23, (global_vars + var.offset));
#line 239 "codegen_ia32_emitter.cpp.dasc"
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + var.offset;
        //| push dword[ebp + offset]
        dasm_put(Dst, 28, offset);
#line 244 "codegen_ia32_emitter.cpp.dasc"
    }
}

void CodeGeneratorIA32::emit_pushi32(const ArgVariant2& varg)
{
    if(auto opt_imm32 = get_imm32(varg, *this))
    {
        emit_pushi32(*opt_imm32);
    }
    else if(is<DecompiledVar>(varg))
    {
        emit_pushi32(get<DecompiledVar>(varg));
    }
    else if(is<DecompiledVarArray>(varg))
    {
        // TODO
        NotImplementedYet();
    }
    else
    {
        throw DynarecUnexpectedValue(varg.which());
    }
}


/////////////////////
// MOVI32

void CodeGeneratorIA32::emit_movi32(RegGuard& reg_dst, int32_t imm32)
{
    //| mov Rd(reg_dst->id), imm32
    dasm_put(Dst, 33, (reg_dst->id), imm32);
#line 275 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_movi32(RegGuard& reg_dst, const DecompiledVar& src)
{
    if(src.global)
    {
        //| mov Rd(reg_dst->id), dword[(global_vars + src.offset)]
        dasm_put(Dst, 38, (reg_dst->id), (global_vars + src.offset));
#line 282 "codegen_ia32_emitter.cpp.dasc"
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + src.offset;
        //| mov Rd(reg_dst->id), dword[ebp + offset]
        dasm_put(Dst, 44, (reg_dst->id), offset);
#line 287 "codegen_ia32_emitter.cpp.dasc"
    }
}

void CodeGeneratorIA32::emit_movi32(RegGuard& reg_dst, const ArgVariant2& src)
{
    if(auto opt_imm32 = get_imm32(src, *this))
    {
        emit_movi32(reg_dst, *opt_imm32);
    }
    else if(is<DecompiledVar>(src))
    {
        emit_movi32(reg_dst, get<DecompiledVar>(src));
    }
    else if(is<DecompiledVarArray>(src))
    {
        // TODO
        NotImplementedYet();
    }
    else
    {
        throw DynarecUnexpectedValue(src.which());
    }
}

void CodeGeneratorIA32::emit_movi32(const DecompiledVar& dst, RegGuard& reg_src)
{
    if(dst.global)
    {
        //| mov dword[(global_vars + dst.offset)], Rd(reg_src->id)
        dasm_put(Dst, 51, (reg_src->id), (global_vars + dst.offset));
#line 316 "codegen_ia32_emitter.cpp.dasc"
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + dst.offset;
        //| mov dword[ebp + offset], Rd(reg_src->id)
        dasm_put(Dst, 57, (reg_src->id), offset);
#line 321 "codegen_ia32_emitter.cpp.dasc"
    }
}

void CodeGeneratorIA32::emit_movi32(const ArgVariant2& dst, const ArgVariant2& src)
{
    if(is<DecompiledVar>(dst))
    {
        emit_movi32(get<DecompiledVar>(dst), src);
    }
    else if(is<DecompiledVarArray>(dst))
    {
        // TODO
        NotImplementedYet();
    }
    else
    {
        throw DynarecUnexpectedValue(dst.which());
    }
}

void CodeGeneratorIA32::emit_movi32(const DecompiledVar& var_dst, int32_t imm32)
{
    if(var_dst.global)
    {
        //| mov dword[(global_vars + var_dst.offset)], imm32
        dasm_put(Dst, 64, (global_vars + var_dst.offset), imm32);
#line 346 "codegen_ia32_emitter.cpp.dasc"
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + var_dst.offset;
        //| mov dword[ebp + offset], imm32
        dasm_put(Dst, 69, offset, imm32);
#line 351 "codegen_ia32_emitter.cpp.dasc"
    }
}


void CodeGeneratorIA32::emit_movi32(const DecompiledVar& var_dst, const ArgVariant2& src)
{
    if(auto opt_imm32 = get_imm32(src, *this))
    {
        emit_movi32(var_dst, *opt_imm32);
    }
    else
    {
        auto rx = this->regalloc(purposes_temp);
        emit_movi32(rx, src);
        emit_movi32(var_dst, rx);
    }
}


/////////////////////
// ADDI32

void CodeGeneratorIA32::emit_addi32(RegGuard& reg_dst, int32_t imm32)
{
    //| mov Rd(reg_dst->id), imm32
    dasm_put(Dst, 33, (reg_dst->id), imm32);
#line 376 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_addi32(const DecompiledVar& var_dst, int32_t imm32)
{
    if(var_dst.global)
    {
        //| add dword[(global_vars + var_dst.offset)], imm32
        dasm_put(Dst, 74, (global_vars + var_dst.offset), imm32);
#line 383 "codegen_ia32_emitter.cpp.dasc"
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + var_dst.offset;
        //| add dword[ebp + offset], imm32
        dasm_put(Dst, 80, offset, imm32);
#line 388 "codegen_ia32_emitter.cpp.dasc"
    }
}

void CodeGeneratorIA32::emit_addi32(const DecompiledVar& dst, RegGuard& reg_src)
{
    if(dst.global)
    {
        //| add dword[(global_vars + dst.offset)], Rd(reg_src->id)
        dasm_put(Dst, 85, (reg_src->id), (global_vars + dst.offset));
#line 396 "codegen_ia32_emitter.cpp.dasc"
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + dst.offset;
        //| add dword[ebp + offset], Rd(reg_src->id)
        dasm_put(Dst, 91, (reg_src->id), offset);
#line 401 "codegen_ia32_emitter.cpp.dasc"
    }
}


void CodeGeneratorIA32::emit_addi32(const DecompiledVar& var_dst, const ArgVariant2& src)
{
    if(auto opt_imm32 = get_imm32(src, *this))
    {
        emit_addi32(var_dst, *opt_imm32);
    }
    else
    {
        auto rx = this->regalloc(purposes_temp);
        emit_movi32(rx, src);
        emit_addi32(var_dst, rx);
    }
}

void CodeGeneratorIA32::emit_addi32(const ArgVariant2& dst, const ArgVariant2& src)
{
    if(is<DecompiledVar>(dst))
    {
        emit_addi32(get<DecompiledVar>(dst), src);
    }
    else if(is<DecompiledVarArray>(dst))
    {
        // TODO
        NotImplementedYet();
    }
    else
    {
        throw DynarecUnexpectedValue(dst.which());
    }
}

