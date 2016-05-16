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
static const unsigned char actions[314] = {
  249,255,252,233,245,250,15,255,232,245,255,195,250,15,255,132,192,240,129,
  240,33,255,15,132,245,255,15,133,245,255,15,133,244,247,232,245,248,1,255,
  15,133,244,247,195,248,1,255,176,1,255,176,0,255,15,148,208,240,34,255,139,
  128,253,240,129,240,1,233,255,137,132,253,240,129,36,233,255,139,4,240,129,
  36,255,252,243,255,164,255,102,165,255,170,255,217,29,237,255,217,157,233,
  255,232,243,255,252,255,208,240,33,255,252,255,21,237,255,252,255,149,233,
  255,104,237,255,80,240,32,255,252,255,53,237,255,252,255,181,233,255,184,
  240,32,237,255,141,133,253,240,129,233,255,102,137,5,240,129,237,255,102,
  199,5,237,236,255,102,199,128,253,240,1,233,236,255,102,137,128,253,240,129,
  240,1,233,255,15,182,192,240,130,240,34,255,137,192,240,129,240,33,255,139,
  5,240,129,237,255,139,133,253,240,129,233,255,137,133,253,240,129,233,255,
  199,5,237,237,255,199,133,233,237,255,199,128,253,240,1,233,237,255,129,192,
  240,33,239,255,129,5,253,237,239,255,129,133,233,239,255,1,5,240,129,237,
  255,1,133,253,240,129,233,255,129,252,248,240,33,239,255,129,61,253,237,239,
  255,129,189,233,239,255,57,5,240,129,237,255,57,133,253,240,129,233,255,129,
  232,240,33,239,255,33,192,240,129,240,33,255,9,192,240,129,240,33,255
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

#undef Dst
#define Dst &this->dstate

static bool is_branch(uint16_t id)
{
    // TODO more
    return id == 0x004C || id == 0x004D;
}

void CodeGeneratorIA32::test()
{
    auto& codegen = *this;

    for(auto it = decompiled.begin(); it != decompiled.end(); )
    {
        it = generate_code(*it, it, codegen);
        //| nop
        //| nop
    }

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
    // End of script mark?
    if(def.offset == SIZE_MAX)
        return ++it;

    auto label_id = codegen.add_label(-def.offset);

    // flush context, the beggining of label should have all the context in CRunningScript
    codegen.emit_flush();
    //| =>(label_id):
    dasm_put(Dst, 0, (label_id));
#line 124 "codegen_ia32_emitter.cpp.dasc"

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
        codegen.emit_flush(); // flush context before going into another script
        codegen.emit_fastcall("DYNAREC_RTL_Wait", tag_CRunningScript, ccmd.args[0]);
        return ++it;
    });

    // GOTO
    this->add_generator(0x0002, [&](const DecompiledCommand& ccmd, IterData it)
    {
        Expects(ccmd.args.size() == 1);
        auto label_id = codegen.add_label(ccmd.args[0]);

        codegen.emit_flush(); // flush before branch
        //| jmp =>(label_id)
        //| .align 16
        dasm_put(Dst, 2, (label_id));
#line 174 "codegen_ia32_emitter.cpp.dasc"

        return ++it;
    });

    // GOSUB
    this->add_generator(0x0050, [&](const DecompiledCommand& ccmd, IterData it)
    {
        Expects(ccmd.args.size() == 1);
        auto label_id = codegen.add_label(ccmd.args[0]);

        codegen.emit_flush(); // flush before branch
        //| call =>(label_id)
        dasm_put(Dst, 8, (label_id));
#line 186 "codegen_ia32_emitter.cpp.dasc"
        return ++it;
    });

    // RETURN
    this->add_generator(0x0051, [&](const DecompiledCommand& ccmd, IterData it)
    {
        Expects(ccmd.args.size() == 0);

        codegen.emit_flush(); // flush before branch
        //| ret
        //| .align 16
        dasm_put(Dst, 11);
#line 197 "codegen_ia32_emitter.cpp.dasc"
        return ++it;
    });

    // GOTO_IF_FALSE / GOTO_IF_TRUE / GOSUB_IF_FALSE / RETURN_IF_FALSE
    {
        auto opgen_branchif = [&](const DecompiledCommand& ccmd, IterData it)
        {
            Expects(ccmd.args.size() == 1);
            auto label_id = codegen.add_label(ccmd.args[0]);

            // flush before branching
            codegen.emit_flush();

            // Use byte test since cmp operations only sets a single byte.
            auto reg_eax = regalloc(Reg::Eax);
            //| test Rb(reg_eax->id), Rb(reg_eax->id) // TODO abstract access?
            dasm_put(Dst, 15, (reg_eax->id), (reg_eax->id));
#line 213 "codegen_ia32_emitter.cpp.dasc"

            if(ccmd.id == 0x004D) // GOTO_IF_FALSE
            {
                //| jz =>(label_id)
                dasm_put(Dst, 22, (label_id));
#line 217 "codegen_ia32_emitter.cpp.dasc"
            }
            else if(ccmd.id == 0x004C) // GOTO_IF_TRUE
            {
                //| jnz =>(label_id)
                dasm_put(Dst, 26, (label_id));
#line 221 "codegen_ia32_emitter.cpp.dasc"
            }
            else if(ccmd.id == 0x0AA0) // GOSUB_IF_FALSE
            {
                // TODO do we need nops here to avoid a pipeline stall?
                //| jnz >1
                //| call =>(label_id)
                //| 1:
                dasm_put(Dst, 30, (label_id));
#line 228 "codegen_ia32_emitter.cpp.dasc"
            }
            else if(ccmd.id == 0x0AA1) // RETURN_IF_FALSE
            {
                // TODO do we need nops here to avoid a pipeline stall?
                //| jnz >1
                //| ret
                //| 1:
                dasm_put(Dst, 39);
#line 235 "codegen_ia32_emitter.cpp.dasc"
            }
            
            return ++it;
        };

        this->add_generator(0x004D, opgen_branchif); // GOTO_IF_FALSE
        this->add_generator(0x004C, opgen_branchif); // GOTO_IF_TRUE (TODO GTA3 only)
        this->add_generator(0x0AA0, opgen_branchif); // GOSUB_IF_FALSE (TODO CLEO only)
        this->add_generator(0x0AA1, opgen_branchif); // RETURN_IF_FALSE (TODO CLEO only)
    }

    // RETURN_TRUE / RETURN_FALSE
    {
        auto opgen_setcond = [&](const DecompiledCommand& ccmd, IterData it)
        {
            Expects(ccmd.args.size() == 0);
            if(ccmd.id == 0x00C5 || ccmd.id == 0x0485) // RETURN_TRUE
            {
                //| mov al, 1  // TODO abstract
                dasm_put(Dst, 47);
#line 254 "codegen_ia32_emitter.cpp.dasc"
            }
            else if(ccmd.id == 0x00C6 || ccmd.id == 0x059A) // RETURN_FALSE
            {
                //| mov al, 0  // TODO abstract
                dasm_put(Dst, 50);
#line 258 "codegen_ia32_emitter.cpp.dasc"
            }
            return ++it;
        };

        this->add_generator(0x00C5, opgen_setcond); // RETURN_TRUE (TODO III ONLY)
        this->add_generator(0x00C6, opgen_setcond); // RETURN_FALSE (TODO III ONLY)
        this->add_generator(0x0485, opgen_setcond); // IS_PC_VERSION / RETURN_TRUE (TODO VC?/SA ONLY - III with CLEO2)
        this->add_generator(0x059A, opgen_setcond); // IS_AUSTRALIAN_GAME / RETURN_FALSE (TODO VC?/SA ONLY - III with CLEO2)
    }

    // TERMINATE_THIS_SCRIPT / TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME / and such
    {
        auto opgen_terminate = [&](const DecompiledCommand& ccmd, IterData it)
        {
            // before terminating, flush context
            codegen.emit_flush();

            switch(ccmd.id)
            {
                case 0x004E: // TERMINATE_THIS_SCRIPT
                    Expects(ccmd.args.size() == 0);
                    codegen.emit_fastcall("DYNAREC_RTL_Terminate", tag_CRunningScript);
                    //| .align 16
                    dasm_put(Dst, 5);
#line 281 "codegen_ia32_emitter.cpp.dasc"
                    break;
                case 0x0A93: // TERMINATE_THIS_CUSTOM_SCRIPT
                case 0x05DC:
                    Expects(ccmd.args.size() == 0);
                    codegen.emit_fastcall("DYNAREC_RTL_TerminateCs", tag_CRunningScript);
                    //| .align 16
                    dasm_put(Dst, 5);
#line 287 "codegen_ia32_emitter.cpp.dasc"
                    break;
                case 0x0459: // TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME
                    Expects(ccmd.args.size() == 1);
                    codegen.emit_fastcall("DYNAREC_RTL_TerminateNamed", tag_CRunningScript, ccmd.args[0]);
                    break;
                case 0x0ABA: // TERMINATE_ALL_CUSTOM_SCRIPTS_WITH_THIS_NAME
                case 0x05DD:
                    Expects(ccmd.args.size() == 1);
                    codegen.emit_fastcall("DYNAREC_RTL_TerminateCsNamed", tag_CRunningScript, ccmd.args[0]);
                    break;
            }
            return ++it;
        };

        this->add_generator(0x004E, opgen_terminate); // TERMINATE_THIS_SCRIPT
        this->add_generator(0x0A93, opgen_terminate); // TERMINATE_THIS_CUSTOM_SCRIPT (TODO CLEO ONLY)
        this->add_generator(0x0459, opgen_terminate); // TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME
        this->add_generator(0x0ABA, opgen_terminate); // TERMINATE_ALL_CUSTOM_SCRIPTS_WITH_THIS_NAME (TODO CLEO ONLY)
        this->add_generator(0x05DC, opgen_terminate); // TERMINATE_THIS_CUSTOM_SCRIPT (TODO III/VC CLEO ONLY)
        this->add_generator(0x05DD, opgen_terminate); // TERMINATE_ALL_CUSTOM_SCRIPTS_WITH_THIS_NAME (TODO III/VC CLEO ONLY)
    }

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

    // ADD_THING_TO_THING
    {
        // TODO opgen_addf

        auto opgen_addi = [&](const DecompiledCommand& ccmd, IterData it)
        {
            Expects(ccmd.args.size() == 2);
            codegen.emit_addi32(ccmd.args[0], ccmd.args[1]);
            return ++it;
        };

        this->add_generator(0x0008, opgen_addi);
        this->add_generator(0x0009, opgen_addi);
        this->add_generator(0x000A, opgen_addi);
        this->add_generator(0x000B, opgen_addi);
        this->add_generator(0x0058, opgen_addi);
        this->add_generator(0x0059, opgen_addi);
        this->add_generator(0x005A, opgen_addi);
        this->add_generator(0x005B, opgen_addi);
        this->add_generator(0x005C, opgen_addi);
        this->add_generator(0x005D, opgen_addi);
        this->add_generator(0x005E, opgen_addi);
        this->add_generator(0x005F, opgen_addi);
    }

    // IS_THING_EQUAL_TO_THING
    {
        // TODO opgen_equf

        auto opgen_equi = [&](const DecompiledCommand& ccmd, IterData it)
        {
            Expects(ccmd.args.size() == 2);
            codegen.emit_cmpi32(ccmd.args[0], ccmd.args[1]);

            // TODO abstract access
            auto reg_eax = regalloc(Reg::Eax);
            //| setz Rb(reg_eax->id)
            dasm_put(Dst, 53, (reg_eax->id));
#line 371 "codegen_ia32_emitter.cpp.dasc"

            return ++it;
        };

        this->add_generator(0x0038, opgen_equi);
        this->add_generator(0x0039, opgen_equi);
        this->add_generator(0x003A, opgen_equi);
        this->add_generator(0x003B, opgen_equi);
        this->add_generator(0x003C, opgen_equi);
        this->add_generator(0x0042, opgen_equi);
        this->add_generator(0x0043, opgen_equi);
        this->add_generator(0x0044, opgen_equi);
        this->add_generator(0x0045, opgen_equi);
        this->add_generator(0x0046, opgen_equi);
        this->add_generator(0x04A3, opgen_equi);
        this->add_generator(0x04A4, opgen_equi);
    }

    

    // ANDOR
    this->add_generator(0x00D6, [&](const DecompiledCommand& ccmd, IterData it)
    {
        Expects(ccmd.args.size() == 1);

        bool is_and;
        size_t num_conds;

        // Find the values for is_and and num_conds...
        if(auto opt_arg = get_imm32(ccmd.args[0], codegen))
        {
            auto arg = *opt_arg;
            if(arg == 0)
            {
                is_and = false;
                num_conds = 1;
            }
            else if(arg >= 1 && arg <= 8)
            {
                is_and = true;
                num_conds = arg + 1;
            }
            else if(arg >= 21 && arg <= 28)
            {
                is_and = false;
                num_conds = (arg - 20) + 1;
            }
            else
            {
                throw DynarecError("ANDOR argument is not zero, 1-8 or 21-28.");
            }
        }
        else
        {
            throw DynarecError("ANDOR argument is not constant");
        }

        // Do nothing if a single condition, let GOTO_IF_FALSE take care of the rest.
        if(num_conds == 1)
            return ++it;
        else
            ++it;

        auto rz = codegen.regalloc(purposes_store);
        Expects(static_cast<Reg>(rz->id) != Reg::Eax);

        for(size_t i = 0; i < num_conds; ++i)
        {
            // Ensure we didn't reach the end of the script, or some other garbage (HEX?)
            if(!is<DecompiledCommand>(it->data))
                throw DynarecUnexpectedValue(it->data.which());

            auto& curr_ccmd = get<DecompiledCommand>(it->data);

            if(is_branch(curr_ccmd.id))
                throw DynarecError("Unexpected branch on ANDOR conditions.");

            if(codegen.run_generator(curr_ccmd, it) != it+1)
                throw DynarecError("ANDOR didn't expect run_generator generating more than one command.");

            auto eax = codegen.regalloc(Reg::Eax);

            if(i == 0)
                codegen.emit_movi32(rz, eax);
            else if(is_and)
                codegen.emit_andi32(rz, eax);
            else
                codegen.emit_ori32(rz, eax);

            ++it;
        }

        // Ensure this next instruction is a branch.
        if(!is<DecompiledCommand>(it->data) || !is_branch(get<DecompiledCommand>(it->data).id))
            throw DynarecError("Expected branch after ANDOR conditions.");

        // Put resulting flags into eax.
        // TODO optimize, when (i+1 == num_conds), use dst=eax!
        codegen.emit_movi32(codegen.regalloc(Reg::Eax), rz);

        return it;
    });

    // CALL
    {
        auto opgen_call = [&](const DecompiledCommand& ccmd, IterData it)
        {
            // TODO try to improve this, oh man...
            // CALL is this fucking terrible because of the nature of SCM for those long years.
            // i.e. we kept writing low-level code directly, so we may have weirdd label indirections
            // local variable access from outer scripts, and lots of weird shit :(

            Expects(ccmd.args.size() >= 2 + 1); //+1 for null terminating argument
            Expects(is<EOAL>(ccmd.args.back()));

            auto label_id = codegen.add_label(ccmd.args[0]);
            size_t num_input_args__;

            if(auto opt_narg = get_imm32(ccmd.args[1], codegen))
                num_input_args__ = *opt_narg;
            else
                throw DynarecError("CALL num arguments is not constant");

            size_t num_inputs = num_input_args__;
            size_t num_outputs = (ccmd.args.size() - (2 + 1)) - num_inputs;

            if(num_inputs > (ccmd.args.size() - (2 + 1)) || ((2 + 1) + num_inputs + num_outputs) != ccmd.args.size())
            {
                throw DynarecError("CALL num arguments is wrong");
            }


            auto& esp = codegen.get_esp();
            auto  rx = codegen.regalloc(purposes_store);

            // make output zone
            codegen.emit_subi32(esp, (num_outputs * 4));
            codegen.emit_movi32(rx, esp);

            // save vars
            codegen.emit_savelocals();

            for(size_t k = 0; k < num_inputs; ++k) // back to front
            {
                auto i = 2 + (k);
                auto lvar_dst = ArgVariant2{ DecompiledVar{ false, k * 4 } };
                codegen.emit_movi32(lvar_dst, ccmd.args[i]);
                // TODO things will go bad above because we're doing a overlapped mov if args[i] is a local var!
                // maybe fetch the args[i] local inputs from savelocals, i dunno.
            }

            // flush before branching and after assigning local vars
            codegen.emit_flush();

            codegen.emit_pushi32(rx); // output zone
            //| call =>(label_id)
            dasm_put(Dst, 8, (label_id));
#line 527 "codegen_ia32_emitter.cpp.dasc"
            codegen.emit_addi32(esp, 4);

            // restore vars
            codegen.emit_restorelocals();

            // take things out of the output zone
            {
                auto rz = codegen.regalloc(purposes_temp);
                for(size_t k = 0; k < num_outputs; ++k) // back to front
                {
                    auto i = 2 + num_inputs + (k);
                    auto& lvar_dst = ccmd.args[i];
                    if(is<DecompiledVar>(lvar_dst))
                    {
                        auto offset = (k * 4);
                        //| mov Rd(rz->id), dword [Rd(rx->id)+offset] // TODO abstract access
                        dasm_put(Dst, 59, (rz->id), (rx->id), offset);
#line 543 "codegen_ia32_emitter.cpp.dasc"
                        codegen.emit_movi32(lvar_dst, rz);
                    }
                    else
                        throw DynarecUnexpectedValue(lvar_dst.which());
                }
            }

            codegen.emit_addi32(esp, (num_outputs * 4));

            return ++it;
        };

        this->add_generator(0x05F5, opgen_call);    // TODO III/VC CLEO ONLY
        this->add_generator(0x0AB1, opgen_call);    // TODO CLEO ONLY
    }

    // RET
    {
        auto opgen_ret = [&](const DecompiledCommand& ccmd, IterData it)
        {
            Expects(ccmd.args.size() >= 1 + 1); //+1 for null terminating argument
            Expects(is<EOAL>(ccmd.args.back()));

            size_t num_outputs;
            
            if(auto opt_narg = get_imm32(ccmd.args[0], codegen))
                num_outputs = *opt_narg;
            else
                throw DynarecError("RET num arguments is not constant");

            if(((1+1) + num_outputs) != ccmd.args.size())
            {
                throw DynarecError("CALL num arguments is wrong");
            }

            // Don't use EAX as temp storage! It may contain a boolean result.
            auto eax_dont_use = codegen.regalloc(Reg::Eax);

            {
                auto rz = codegen.regalloc(purposes_temp);
                for(size_t k = 0; k < num_outputs; ++k)
                {
                    auto i = 1+k;
                    auto offset = (k * 4) + 4/*retptr*/;;
                    codegen.emit_movi32(rz, ccmd.args[i]);
                    //| mov [esp+offset], Rd(rz->id) // TODO abstract access
                    dasm_put(Dst, 68, (rz->id), offset);
#line 589 "codegen_ia32_emitter.cpp.dasc"
                }
            }

            //| ret
            //| .align 16
            dasm_put(Dst, 11);
#line 594 "codegen_ia32_emitter.cpp.dasc"

            return ++it;
        };

        this->add_generator(0x05F6, opgen_ret);    // TODO III/VC CLEO ONLY
        this->add_generator(0x0AB2, opgen_ret);    // TODO CLEO ONLY
    }

    // CALL_ASM
    {
        auto opgen_callasm = [&](const DecompiledCommand& ccmd, IterData it)
        {
            ArgVariant2 none_arg;
            size_t curr_param = 0;
            bool has_return;
            bool is_method;
            
            switch(ccmd.id)
            {
                case 0x0AA5: // CALL_FUNCTION
                case 0x05E1:
                    has_return = false;
                    is_method  = false;
                    break;
                case 0x0AA6: // CALL_METHOD
                case 0x05E3:
                    has_return = false;
                    is_method  = true;
                    break;
                case 0x0AA7: // CALL_FUNCTION_RETURN
                case 0x05E2:
                    has_return = true;
                    is_method  = false;
                    break;
                case 0x0AA8: // CALL_METHOD_RETURN
                case 0x05E4:
                    has_return = true;
                    is_method  = true;
                    break;
                default:
                    Unreachable();
            }

            size_t min_params = is_method? 4 : 3;
            Expects(ccmd.args.size() >= min_params+1); // +1 for null argument
            Expects(is<EOAL>(ccmd.args.back()));

            size_t num_push, num_pop;
            auto& arg_func_ptr = ccmd.args[curr_param++];
            auto& arg_this_ptr = is_method? ccmd.args[curr_param++] : none_arg;
            auto& opt_num_push = get_imm32(ccmd.args[curr_param++], codegen);
            auto& opt_num_pop  = get_imm32(ccmd.args[curr_param++], codegen);

            if(!opt_num_push || !opt_num_pop)
                throw DynarecError("CALL_ASM push/pop arguments is not constant");
           
            num_push = *opt_num_push;
            num_pop  = *opt_num_pop;

            if(ccmd.args.size() != (min_params+1) + num_push + (has_return? 1 : 0))
                throw DynarecError("CALL_ASM num arguments is wrong");

            for(size_t k = 0; k < num_push; ++k)
            {
                codegen.emit_pushi32(ccmd.args[curr_param++]);
            }

            if(is_method)
            {
                auto reg_ecx = codegen.regalloc(Reg::Ecx);
                codegen.emit_movi32(reg_ecx, arg_this_ptr);
                codegen.emit_call(arg_func_ptr);
            }
            else
            {
                codegen.emit_call(arg_func_ptr);
            }

            if(has_return)
            {
                auto  reg_eax = codegen.regalloc(Reg::Eax);
                auto& arg_out = ccmd.args[curr_param++];
                if(is<DecompiledVar>(arg_out))
                    codegen.emit_movi32(arg_out, reg_eax);
                else
                    throw DynarecUnexpectedValue(arg_out.which());
            }

            codegen.emit_addi32(codegen.get_esp(), num_pop * 4);

            return ++it;
        };

        this->add_generator(0x0AA5, opgen_callasm);    // TODO CLEO ONLY 
        this->add_generator(0x05E1, opgen_callasm);    // TODO III/VC CLEO ONLY
        this->add_generator(0x0AA6, opgen_callasm);    // TODO CLEO ONLY
        this->add_generator(0x05E3, opgen_callasm);    // TODO III/VC CLEO ONLY
        this->add_generator(0x0AA7, opgen_callasm);    // TODO CLEO ONLY
        this->add_generator(0x05E2, opgen_callasm);    // TODO III/VC CLEO ONLY
        this->add_generator(0x0AA8, opgen_callasm);    // TODO CLEO ONLY
        this->add_generator(0x05E4, opgen_callasm);    // TODO III/VC CLEO ONLY
    }

    // POP_FLOAT
    {
        auto opgen_fstp = [&](const DecompiledCommand& ccmd, IterData it)
        {
            Expects(ccmd.args.size() == 1);

            if(is<DecompiledVar>(ccmd.args[0]))
            {
                auto& var = get<DecompiledVar>(ccmd.args[0]);
                codegen.emit_fstp(var);
            }
            else
            {
                throw DynarecUnexpectedValue(ccmd.args[0].which());
            }

            return ++it;
        };

        this->add_generator(0x05F2, opgen_fstp);    // TODO III/VC CLEO ONLY
        this->add_generator(0x0AE9, opgen_fstp);    // TODO CLEO ONLY
    }

    // WRITE_MEMORY
    {
        auto op_pwrite = [&](const DecompiledCommand& ccmd, IterData it)
        {
            Expects(ccmd.args.size() == 4);

            auto& dest    = ccmd.args[0];
            auto& value   = ccmd.args[2];
            auto vp_imm32 = get_imm32(ccmd.args[3], codegen);
            auto size_imm32 = get_imm32(ccmd.args[1], codegen);

            if(!size_imm32 || vp_imm32.value_or(1) != 0)
            {
                // Either size isn't constant, vp isn't constant, or vp=false
                codegen.emit_ccall("DYNAREC_RTL_WriteMemory", ccmd.args[0], ccmd.args[1], ccmd.args[2], ccmd.args[3]);
            }
            else
            {
                // If we are here, vp=false and and size is a constant.
                switch(*size_imm32)
                {
                    case 4:
                        // TODO ptr()
                        codegen.emit_movi32(ptr(dest), value);
                        break;
                    case 2:
                        codegen.emit_movu16_from_u32(ptr(dest), value); // treat as unsigned
                        break;
                    default:
                        codegen.emit_memset(dest, *size_imm32, value);
                        break;
                }
            }

            return ++it;
        };

        this->add_generator(0x05DF, op_pwrite);    // TODO III/VC CLEO ONLY
        this->add_generator(0x0A8C, op_pwrite);    // TODO CLEO ONLY
    }



/*
    // CREATE_CHAR
    this->add_generator(0x009A, [&](const DecompiledCommand& ccmd, IterData it)
    {
        static const auto num_rets = 1;

        Expects(ccmd.args.size() == 6);
        auto& esp = codegen.get_esp();
        codegen.emit_subi32(esp, (num_rets * 4));

        for(auto arg = ccmd.args.rbegin() + num_rets, end = ccmd.args.rend();
            arg != end; ++arg)
        {
            codegen.emit_push(*arg);
        }

        codegen.emit_pushi32(esp);
        codegen.emit_pushi32(tag_CRunningScript);
        codegen.emit_ccall("native");
        codegen.emit_addi32(esp, 4);

        {
        auto eax__ = regalloc(Reg::Eax);
        auto ry  = regalloc(purposes_temp);
        //| mov Rd(ry->id), [esp+0] // TODO mov abstraction
        dasm_put(Dst, 76, (ry->id));
#line 788 "codegen_ia32_emitter.cpp.dasc"

        if(!is<DecompiledVar>(ccmd.args[5]))
            throw DynarecUnexpectedValue(ccmd.args[5].which());

        codegen.emit_movi32(get<DecompiledVar>(ccmd.args[5]), ry);
        }

        codegen.emit_addi32(esp, ccmd.args.size() * 4);

        return ++it;
    });
*/
}


///////////////////////////////////////////////
// Emitters
///////////////////////////////////////////////

#undef Dst
#define Dst &this->dstate



void CodeGeneratorIA32::emit_savelocals()
{
    const auto size_bytes = sizeof(CRunningScript::tls);
    auto src_begin = ArgVariant2{ DecompiledVar{ false, 0 * 4 } };
    emit_subi32(get_esp(), size_bytes);
    // TODO lea src_begin!
    emit_memcpy(lea(src_begin), get_esp(), size_bytes);
}

void CodeGeneratorIA32::emit_restorelocals()
{
    const auto size_bytes = sizeof(CRunningScript::tls);
    auto dst_begin = ArgVariant2{ DecompiledVar{ false, 0 * 4 } };
    // TODO lea dst_begin!
    emit_memcpy(get_esp(), lea(dst_begin), size_bytes);
    emit_addi32(get_esp(), size_bytes);
}




void CodeGeneratorIA32::emit_rep()
{
    // TODO when we implement reg caches, remember this modifies ECX
    //| rep
    dasm_put(Dst, 82);
#line 837 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_movsb()
{
    // TODO when we implement reg caches, remember this modifies ESI,EDI
    //| movsb
    dasm_put(Dst, 85);
#line 843 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_movsw()
{
    // TODO when we implement reg caches, remember this modifies ESI,EDI
    //| movsw
    dasm_put(Dst, 87);
#line 849 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_movsd()
{
    // TODO when we implement reg caches, remember this modifies ESI,EDI
    //| movsd
    dasm_put(Dst, 88);
#line 855 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_stosb()
{
    // TODO when we implement reg caches, remember this modifies EDI
    //| stosb
    dasm_put(Dst, 90);
#line 861 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_fstp(const DecompiledVar& var)
{
    if(var.global)
    {
        //| fstp dword[(global_vars + var.offset)]
        dasm_put(Dst, 92, (global_vars + var.offset));
#line 868 "codegen_ia32_emitter.cpp.dasc"
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + var.offset;
        //| fstp dword[ebp + offset]
        dasm_put(Dst, 96, offset);
#line 873 "codegen_ia32_emitter.cpp.dasc"
    }
}




/////////////////////
// CALL

void CodeGeneratorIA32::emit_call(int32_t target_ptr)
{
    //| call &target_ptr
    dasm_put(Dst, 100, (ptrdiff_t)(target_ptr));
#line 885 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_call(RegGuard& reg)
{
    //| call Rd(reg->id)
    dasm_put(Dst, 103, (reg->id));
#line 890 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_call(const DecompiledVar& var)
{
    if(var.global)
    {
        //| call dword [(global_vars + var.offset)]
        dasm_put(Dst, 109, (global_vars + var.offset));
#line 897 "codegen_ia32_emitter.cpp.dasc"
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + var.offset;
        //| call dword [ebp + offset]
        dasm_put(Dst, 114, offset);
#line 902 "codegen_ia32_emitter.cpp.dasc"
    }
}

void CodeGeneratorIA32::emit_call(const ArgVariant2& varg)
{
    if(auto opt_imm32 = get_imm32(varg, *this))
    {
        emit_call(*opt_imm32);
    }
    else if(is<DecompiledVar>(varg))
    {
        emit_call(get<DecompiledVar>(varg));
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
// PUSHI32

void CodeGeneratorIA32::emit_pushi32(int32_t imm32)
{
    //| push imm32
    dasm_put(Dst, 119, imm32);
#line 933 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_pushi32(RegGuard& reg)
{
    //| push Rd(reg->id)
    dasm_put(Dst, 122, (reg->id));
#line 938 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_pushi32(const DecompiledVar& var)
{
    if(var.global)
    {
        //| push dword[(global_vars + var.offset)]
        dasm_put(Dst, 126, (global_vars + var.offset));
#line 945 "codegen_ia32_emitter.cpp.dasc"
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + var.offset;
        //| push dword[ebp + offset]
        dasm_put(Dst, 131, offset);
#line 950 "codegen_ia32_emitter.cpp.dasc"
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
// LEA

void CodeGeneratorIA32::emit_lea(RegGuard& reg_dst, const DecompiledVar& src)
{
    if(src.global)
    {
        //| mov Rd(reg_dst->id), (global_vars + src.offset)
        dasm_put(Dst, 136, (reg_dst->id), (global_vars + src.offset));
#line 983 "codegen_ia32_emitter.cpp.dasc"
        // ^ yes, this is a mov, not a lea
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + src.offset;
        //| lea Rd(reg_dst->id), dword[ebp + offset]
        dasm_put(Dst, 141, (reg_dst->id), offset);
#line 989 "codegen_ia32_emitter.cpp.dasc"
    }
}

void CodeGeneratorIA32::emit_lea(RegGuard& reg_dst, const ArgVariant2& src)
{
    if(is<DecompiledVar>(src))
    {
        emit_lea(reg_dst, get<DecompiledVar>(src));
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

/////////////////////
// MOVU16_FROM_U32


void CodeGeneratorIA32::emit_movu16_from_u32(PointedBy<uintptr_t> p_dst, RegGuard& reg_src)
{
    //| mov word [p_dst.value], Rw(reg_src->id)
    dasm_put(Dst, 148, (reg_src->id), p_dst.value);
#line 1016 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_movu16_from_u32(PointedBy<uintptr_t> p_dst, uint32_t imm32)
{
    //| mov word [p_dst.value], imm32
    dasm_put(Dst, 155, p_dst.value, imm32);
#line 1021 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_movu16_from_u32(PointedBy<uintptr_t> p_dst, const ArgVariant2& src)
{
    if(auto xsrc_imm32 = get_imm32(src, *this))
    {
        emit_movu16_from_u32(p_dst, *xsrc_imm32);
    }
    else
    {
        auto rx = this->regalloc(purposes_temp);
        emit_movi32(rx, src);
        emit_movu16_from_u32(p_dst, rx);
    }
}

void CodeGeneratorIA32::emit_movu16_from_u32(PointedBy<RegGuard&> p_reg_dst, uint32_t imm32)
{
    //| mov word [Rd(p_reg_dst.value->id)], imm32
    dasm_put(Dst, 161, (p_reg_dst.value->id), 0, imm32);
#line 1040 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_movu16_from_u32(PointedBy<RegGuard&> p_reg_dst, RegGuard& reg_src)
{
    //| mov word [Rd(p_reg_dst.value->id)], Rw(reg_src->id)
    dasm_put(Dst, 170, (reg_src->id), (p_reg_dst.value->id), 0);
#line 1045 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_movu16_from_u32(PointedBy<RegGuard&> p_reg_dst, const ArgVariant2& src)
{
    if(auto opt_imm32 = get_imm32(src, *this))
    {
        emit_movu16_from_u32(p_reg_dst, *opt_imm32);
    }
    else
    {
        auto rx = this->regalloc(purposes_temp);
        emit_movi32(rx, src);
        emit_movu16_from_u32(p_reg_dst, rx);
    }
}

void CodeGeneratorIA32::emit_movu16_from_u32(PointedBy<const ArgVariant2&> p_dst, const ArgVariant2& src)
{
    if(auto xdst_imm32 = get_imm32(p_dst.value, *this))
    {
        emit_movu16_from_u32(ptr(*xdst_imm32), src);
    }
    else
    {
        auto rp = this->regalloc(purposes_temp);
        emit_movi32(rp, p_dst.value);
        emit_movu16_from_u32(ptr(rp), src);
    }
}


/////////////////////
// MOVI32_FROM_U8

void CodeGeneratorIA32::emit_movi32_from_u8(RegGuard& reg_dst, RegGuard& reg_src)
{
    //| movzx Rd(reg_dst->id), Rb(reg_src->id)
    dasm_put(Dst, 180, (reg_dst->id), (reg_src->id));
#line 1082 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_movi32_from_u8(RegGuard& reg_dst, const ArgVariant2& src)
{
    if(auto opt_imm32 = get_imm32(src, *this))
    {
        emit_movi32(reg_dst, *opt_imm32);
    }
    else
    {
        auto rx = this->regalloc(purposes_temp);
        emit_movi32(rx, src);
        emit_movi32_from_u8(reg_dst, rx);
    }
}


/////////////////////
// MOVI32

void CodeGeneratorIA32::emit_movi32(RegGuard& reg_dst, int32_t imm32)
{
    //| mov Rd(reg_dst->id), imm32
    dasm_put(Dst, 136, (reg_dst->id), imm32);
#line 1105 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_movi32(RegGuard& reg_dst, RegGuard& reg_src)
{
    //| mov Rd(reg_dst->id), Rd(reg_src->id)
    dasm_put(Dst, 188, (reg_src->id), (reg_dst->id));
#line 1110 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_movi32(RegGuard& reg_dst, tag_CRunningScript_t)
{
    return emit_movi32(reg_dst, this->get_ctx());
}

void CodeGeneratorIA32::emit_movi32(RegGuard& reg_dst, const DecompiledVar& src)
{
    if(src.global)
    {
        //| mov Rd(reg_dst->id), dword[(global_vars + src.offset)]
        dasm_put(Dst, 195, (reg_dst->id), (global_vars + src.offset));
#line 1122 "codegen_ia32_emitter.cpp.dasc"
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + src.offset;
        //| mov Rd(reg_dst->id), dword[ebp + offset]
        dasm_put(Dst, 201, (reg_dst->id), offset);
#line 1127 "codegen_ia32_emitter.cpp.dasc"
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
        dasm_put(Dst, 149, (reg_src->id), (global_vars + dst.offset));
#line 1156 "codegen_ia32_emitter.cpp.dasc"
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + dst.offset;
        //| mov dword[ebp + offset], Rd(reg_src->id)
        dasm_put(Dst, 208, (reg_src->id), offset);
#line 1161 "codegen_ia32_emitter.cpp.dasc"
    }
}

void CodeGeneratorIA32::emit_movi32(const DecompiledVar& var_dst, int32_t imm32)
{
    if(var_dst.global)
    {
        //| mov dword[(global_vars + var_dst.offset)], imm32
        dasm_put(Dst, 215, (global_vars + var_dst.offset), imm32);
#line 1169 "codegen_ia32_emitter.cpp.dasc"
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + var_dst.offset;
        //| mov dword[ebp + offset], imm32
        dasm_put(Dst, 220, offset, imm32);
#line 1174 "codegen_ia32_emitter.cpp.dasc"
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

//-- PointedBy

void CodeGeneratorIA32::emit_movi32(PointedBy<uintptr_t> p_dst, RegGuard& reg_src)
{
    //| mov dword [p_dst.value], Rd(reg_src->id)
    dasm_put(Dst, 149, (reg_src->id), p_dst.value);
#line 1196 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_movi32(PointedBy<uintptr_t> p_dst, int32_t src)
{
    //| mov dword [p_dst.value], src
    dasm_put(Dst, 215, p_dst.value, src);
#line 1201 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_movi32(PointedBy<RegGuard&> p_reg_dst, RegGuard& reg_src)
{
    //| mov dword[Rd(p_reg_dst.value->id)], Rd(reg_src->id)
    dasm_put(Dst, 171, (reg_src->id), (p_reg_dst.value->id), 0);
#line 1206 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_movi32(PointedBy<RegGuard&> p_reg_dst, int32_t imm32)
{
    //| mov dword[Rd(p_reg_dst.value->id)], imm32
    dasm_put(Dst, 225, (p_reg_dst.value->id), 0, imm32);
#line 1211 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_movi32(PointedBy<uintptr_t> p_dst, const ArgVariant2& src)
{
    if(auto opt_xsrc_imm32 = get_imm32(src, *this))
    {
        emit_movi32(p_dst, *opt_xsrc_imm32);
    }
    else
    {
        auto ry = regalloc(purposes_temp);
        emit_movi32(ry, src);
        emit_movi32(p_dst, ry);
    }
}

void CodeGeneratorIA32::emit_movi32(PointedBy<RegGuard&> p_reg_dst, const ArgVariant2& src)
{
    if(auto opt_src_imm32 = get_imm32(src, *this))
    {
        emit_movi32(p_reg_dst, *opt_src_imm32);
    }
    else
    {
        auto ry = regalloc(purposes_temp);
        emit_movi32(ry, src);
        emit_movi32(p_reg_dst, ry);
    }
}

void CodeGeneratorIA32::emit_movi32(PointedBy<const ArgVariant2&> p_var_dst, const ArgVariant2& src)
{
    if(auto opt_dst_imm32 = get_imm32(p_var_dst.value, *this))
    {
        emit_movi32(ptr(*opt_dst_imm32), src);
    }
    else
    {
        auto rx = regalloc(purposes_temp);
        emit_movi32(rx, p_var_dst.value);
        emit_movi32(ptr(rx), src);
    }
}




/////////////////////
// ADDI32

void CodeGeneratorIA32::emit_addi32(RegGuard& reg_dst, int32_t imm32)
{
    //| add Rd(reg_dst->id), imm32
    dasm_put(Dst, 233, (reg_dst->id), imm32);
#line 1264 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_addi32(const DecompiledVar& var_dst, int32_t imm32)
{
    if(var_dst.global)
    {
        //| add dword[(global_vars + var_dst.offset)], imm32
        dasm_put(Dst, 239, (global_vars + var_dst.offset), imm32);
#line 1271 "codegen_ia32_emitter.cpp.dasc"
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + var_dst.offset;
        //| add dword[ebp + offset], imm32
        dasm_put(Dst, 245, offset, imm32);
#line 1276 "codegen_ia32_emitter.cpp.dasc"
    }
}

void CodeGeneratorIA32::emit_addi32(const DecompiledVar& dst, RegGuard& reg_src)
{
    if(dst.global)
    {
        //| add dword[(global_vars + dst.offset)], Rd(reg_src->id)
        dasm_put(Dst, 250, (reg_src->id), (global_vars + dst.offset));
#line 1284 "codegen_ia32_emitter.cpp.dasc"
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + dst.offset;
        //| add dword[ebp + offset], Rd(reg_src->id)
        dasm_put(Dst, 256, (reg_src->id), offset);
#line 1289 "codegen_ia32_emitter.cpp.dasc"
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


/////////////////////
// CMPI32

void CodeGeneratorIA32::emit_cmpi32(RegGuard& a, int32_t b)
{
    //| cmp Rd(a->id), b
    dasm_put(Dst, 263, (a->id), b);
#line 1331 "codegen_ia32_emitter.cpp.dasc"
}

void CodeGeneratorIA32::emit_cmpi32(const DecompiledVar& a, int32_t b)
{
    if(a.global)
    {
        //| cmp dword[(global_vars + a.offset)], b
        dasm_put(Dst, 270, (global_vars + a.offset), b);
#line 1338 "codegen_ia32_emitter.cpp.dasc"
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + a.offset;
        //| cmp dword[ebp + offset], b
        dasm_put(Dst, 276, offset, b);
#line 1343 "codegen_ia32_emitter.cpp.dasc"
    }
}

void CodeGeneratorIA32::emit_cmpi32(const DecompiledVar& a, RegGuard& b)
{
    if(a.global)
    {
        //| cmp dword[(global_vars + a.offset)], Rd(b->id)
        dasm_put(Dst, 281, (b->id), (global_vars + a.offset));
#line 1351 "codegen_ia32_emitter.cpp.dasc"
    }
    else
    {
        auto offset = offsetof(CRunningScript, tls) + a.offset;
        //| cmp dword[ebp + offset], Rd(b->id)
        dasm_put(Dst, 287, (b->id), offset);
#line 1356 "codegen_ia32_emitter.cpp.dasc"
    }
}


void CodeGeneratorIA32::emit_cmpi32(const DecompiledVar& a, const ArgVariant2& b)
{
    if(auto opt_imm32 = get_imm32(b, *this))
    {
        emit_cmpi32(a, *opt_imm32);
    }
    else
    {
        auto rx = this->regalloc(purposes_temp);
        emit_movi32(rx, b);
        emit_cmpi32(a, rx);
    }
}

void CodeGeneratorIA32::emit_cmpi32(const ArgVariant2& a, const ArgVariant2& b)
{
    if(is<DecompiledVar>(a))
    {
        emit_cmpi32(get<DecompiledVar>(a), b);
    }
    else if(is<DecompiledVarArray>(a))
    {
        // TODO
        NotImplementedYet();
    }
    else
    {
        throw DynarecUnexpectedValue(a.which());
    }
}


/////////////////////
// SUBI32

void CodeGeneratorIA32::emit_subi32(RegGuard& reg_dst, int32_t imm32)
{
    //| sub Rd(reg_dst->id), imm32
    dasm_put(Dst, 294, (reg_dst->id), imm32);
#line 1398 "codegen_ia32_emitter.cpp.dasc"
}

/////////////////////
// ANDI32

void CodeGeneratorIA32::emit_andi32(RegGuard& reg_dst, RegGuard& reg_src)
{
    //| and Rd(reg_dst->id), Rd(reg_src->id)
    dasm_put(Dst, 300, (reg_src->id), (reg_dst->id));
#line 1406 "codegen_ia32_emitter.cpp.dasc"
}

/////////////////////
// ORI32

void CodeGeneratorIA32::emit_ori32(RegGuard& reg_dst, RegGuard& reg_src)
{
    //| or Rd(reg_dst->id), Rd(reg_src->id)
    dasm_put(Dst, 307, (reg_src->id), (reg_dst->id));
#line 1414 "codegen_ia32_emitter.cpp.dasc"
}
