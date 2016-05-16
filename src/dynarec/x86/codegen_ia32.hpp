#include "../../stdinc.h"
#include "../../disassembler.hpp"

struct CodeGeneratorIA32;

/// Gets the immediate 32 bits value of the value inside the variant, or nullopt if not possible.
///
/// This function should be overloaded/specialized for each possible value in ArgVariant2.
///
template<typename T>
static optional<int32_t> get_imm32(const T&, CodeGeneratorIA32& codegen);
static optional<int32_t> get_imm32(const ArgVariant2&, CodeGeneratorIA32& codegen);

/// Converts intermediate representation (given by `Disassembler`) into x86.
struct CodeGeneratorIA32
{
public:
    // Forwarding
    struct RegGuard;
    struct RegState;
    
    template<typename T>
    struct AddressOf;

    template<typename T>
    struct PointedBy;

    // Tag to use the CRunningScript context in a instruction.
    struct tag_CRunningScript_t {};

    /// Self parameter for DASM
    struct Dst_DECL_t
    {
        CodeGeneratorIA32& codegen;
        struct dasm_State* dasm_state;
    };

    /// Id for x86 registers.
    enum class Reg : uint8_t
    {
        // Ids must match the rN pattern of x86-64.
        // See https://corsix.github.io/dynasm-doc/instructions.html
        Eax = 0,
        Ecx = 1,
        Edx = 2,
        Ebx = 3,
        Esp = 4,
        Ebp = 5,
        Esi = 6,
        Edi = 7,
        Max_,
    };

private:
    // Inputs
    const Commands&             commands;
    std::vector<DecompiledData> decompiled;
    
    /// Pointer to the global variable space in the process.
    char* global_vars;

    // DASM label register
    void                            *dlabels[1];    //< Buffer to store offset of the global DASM labels.
    unsigned int                    dstate_maxpc;   //< Current amount of DASM labels used.
    std::map<int32_t, unsigned int> pcmap;          //< Map of <scm_offset, dasm_label_id>

public:
    /// State of the DASM assembler.
    Dst_DECL_t dstate;

    /// Iterator of decompiled SCM data.
    using IterData = decltype(decompiled)::iterator;

    /// Prototype for x86 generators.
    using opgen_func_t = std::function<IterData(const DecompiledCommand&, IterData)>;

public:
    void test();

    CodeGeneratorIA32(const Commands& commands, std::vector<DecompiledData> decompiled);
    CodeGeneratorIA32(const CodeGeneratorIA32&) = delete;
    CodeGeneratorIA32(CodeGeneratorIA32&&) = default;
    ~CodeGeneratorIA32();

    /// Returns the address to the external symbol named `extern_name`.
    /// If `is_rel` is true, returns a address relative from `addr+4`, otherwise absolute address.
    int32_t resolve_extern(unsigned char* addr, const char* extern_name, bool is_rel);

    /// Runs generator for the specified command.
    ///
    /// Returns the next command to generate, usually simply `++it`.
    IterData run_generator(const DecompiledCommand& ccmd, IterData it);

    /// Adds, or finds if already added, a DASM label id associated with the `label_param`.
    /// `label_param` is the label offset as given in a SCM script.
    unsigned int add_label(int32_t label_param);

    /// Adds, or finds if already added, a DASM label id associated with the specified SCM parameter.
    unsigned int add_label(const ArgVariant2& varg);





    ///////////////////////////////////////////
    // Register Allocator
    ///////////////////////////////////////////

    /// Very temporary usage, like to intermediate a arithmetic operation and such.
    static constexpr int purposes_temp  = 1;
    /// Anything else that's not `purposes_temp`.
    static constexpr int purposes_store = 2;

    /// Allocates a register for the specified purposes.
    /// See the `purposes_*` constants.
    RegGuard regalloc(int purposes);
    
    /// Forces the allocates of the specified register.
    RegGuard regalloc(Reg reg);

    /// Gets the ESP register.
    RegGuard& get_esp()
    {
        return this->reg_esp;
    }

    /// Gets the register that stores the CRunningScript context.
    RegGuard& get_ctx()
    {
        return this->reg_ctx;
    }

    /// Flushes the CRunningScript context.
    ///
    /// All variables cached in registers will be put back into their original locations.
    void emit_flush()
    {
        // Flush CRunningScript context/variables.
        // Currently we don't have a proper reg cache, so we don't need to flush anything.
    }

    /// Flushes the temporary registers EAX, ECX and EDX.
    ///
    /// That is, if those registers are caching anything, put them back into their original location.
    /// This must be called before calling functions that follow the x86 ABI, i.e. doesn't preserve EAX, ECX, EDX.
    void emit_flush_temp_regs()
    {
        // Flush temporary registers.
        // Currently we don't have a proper reg cache, so we don't need to flush anything.
        ///Let's just ensure those regs aren't being used.
        Expects(this->regstates[static_cast<size_t>(Reg::Eax)].is_allocated == false);
        Expects(this->regstates[static_cast<size_t>(Reg::Ecx)].is_allocated == false);
        Expects(this->regstates[static_cast<size_t>(Reg::Edx)].is_allocated == false);
    }

    void emit_savelocals();
    void emit_restorelocals();


    ///////////////////////////////////////////
    // Instruction Emitter
    ///////////////////////////////////////////

    // See also emit_flush
    // See also emit_flush_temp_regs

    // Abstracted functionalities:

    /// Emits code equivalent to C++'s std::memcpy(dst, src, count)
    template<typename TDst, typename TSrc, typename TCount>
    void emit_memcpy(TDst&& dst, TSrc&& src, TCount&& count);

    /// Emits code equivalent to C++'s std::memset(dst, value, count), including
    /// using only the first 8 bits of value.
    template<typename TDst, typename TValue, typename TCount>
    void emit_memset(TDst&& dst, TValue&& value, TCount&& count);

    // Literal emitters:
    // They do NOTHING but emit the specified instruction.
    void emit_rep();
    void emit_movsb();
    void emit_movsw();
    void emit_movsd();
    void emit_stosb();
    void emit_fstp(const DecompiledVar& var);

    // x86 CALL
    void emit_call(int32_t target_ptr);
    void emit_call(RegGuard& reg);
    void emit_call(const DecompiledVar& var);
    void emit_call(const ArgVariant2& varg);

    // x86 PUSH
    void emit_pushi32(int32_t imm32);
    void emit_pushi32(RegGuard& reg);
    void emit_pushi32(const DecompiledVar& var);
    void emit_pushi32(const ArgVariant2& varg);

    // x86 LEA
    void emit_lea(RegGuard& dst, const DecompiledVar& src);
    void emit_lea(RegGuard& dst, const ArgVariant2& src);

    // abstract MOVU16_FROM_U32 (TODO if this gets too bloated remove and simplifyyyyy)
    void emit_movu16_from_u32(PointedBy<uintptr_t> p_dst, RegGuard& reg_src);
    void emit_movu16_from_u32(PointedBy<uintptr_t> p_dst, uint32_t imm32);
    void emit_movu16_from_u32(PointedBy<uintptr_t> p_dst, const ArgVariant2& src);
    void emit_movu16_from_u32(PointedBy<RegGuard&> p_reg_dst, uint32_t imm32);
    void emit_movu16_from_u32(PointedBy<RegGuard&> p_reg_dst, RegGuard& reg_src);
    void emit_movu16_from_u32(PointedBy<RegGuard&> p_reg_dst, const ArgVariant2& src);
    void emit_movu16_from_u32(PointedBy<const ArgVariant2&> p_dst, const ArgVariant2& src);

    // abstract MOVI32_FROM_U8 (TODO if this gets too bloated remove and simplifyyyyy)
    void emit_movi32_from_u8(RegGuard& reg_dst, RegGuard& reg_src);
    void emit_movi32_from_u8(RegGuard& reg_dst, const ArgVariant2& src);

    // x86 MOV
    void emit_movi32(RegGuard& dst, int32_t imm32);
    void emit_movi32(RegGuard& dst, tag_CRunningScript_t);
    void emit_movi32(RegGuard& dst, RegGuard& reg_src);
    void emit_movi32(RegGuard& dst, const DecompiledVar& src);
    void emit_movi32(RegGuard& dst, const ArgVariant2& src);
    void emit_movi32(const DecompiledVar& dst, int32_t imm32);
    void emit_movi32(const DecompiledVar& dst, RegGuard& src);
    void emit_movi32(const DecompiledVar& dst, const ArgVariant2& src);
    template<typename TSrc>
    void emit_movi32(const ArgVariant2& dst, TSrc& src);
    template<typename TSrcAddr>
    void emit_movi32(RegGuard& reg_dst, AddressOf<TSrcAddr> src);
    //
    void emit_movi32(PointedBy<uintptr_t> p_dst, RegGuard& reg_src);
    void emit_movi32(PointedBy<uintptr_t> p_dst, int32_t src);
    void emit_movi32(PointedBy<uintptr_t> p_dst, const ArgVariant2& src);
    void emit_movi32(PointedBy<RegGuard&> p_reg_dst, int32_t imm32);
    void emit_movi32(PointedBy<RegGuard&> p_reg_dst, RegGuard& reg_src);
    void emit_movi32(PointedBy<RegGuard&> p_reg_dst, const ArgVariant2& src);
    void emit_movi32(PointedBy<const ArgVariant2&> p_var_dst, const ArgVariant2& src);


    // x86 ADD
    void emit_addi32(RegGuard& dst, int32_t imm32);
    void emit_addi32(const DecompiledVar& dst, const ArgVariant2& src);
    void emit_addi32(const DecompiledVar& dst, int32_t imm32);
    void emit_addi32(const DecompiledVar& dst, RegGuard& reg_src);
    void emit_addi32(const ArgVariant2& dst, const ArgVariant2& src);

    // x86 SUB
    void emit_subi32(RegGuard& dst, int32_t imm32);

    // x86 CMP
    void emit_cmpi32(RegGuard& a, int32_t b);
    void emit_cmpi32(const DecompiledVar& a, const ArgVariant2& b);
    void emit_cmpi32(const DecompiledVar& a, int32_t b);
    void emit_cmpi32(const DecompiledVar& a, RegGuard& b);
    void emit_cmpi32(const ArgVariant2& a, const ArgVariant2& b);

    // x86 AND
    void emit_andi32(RegGuard& dst, RegGuard& src);

    // x86 OR
    void emit_ori32(RegGuard& dst, RegGuard& src);

    /// Generic PUSH.
    void emit_push(const ArgVariant2& varg)
    {
        return emit_pushi32(varg);
    }

    /// Generic PUSH.
    /// Pushes the current CRunningScript context.
    void emit_push(tag_CRunningScript_t)
    {
        return emit_pushi32(this->reg_ctx);
    }

    /// Emits a call to a __cdecl function.
    /// This emitter pushes all arguments, flushes temporary registers, as well as clean ups the stack after the call.
    template<typename... Args>
    void emit_ccall(const char* extern_name, Args&&... args)
    {
        auto target_ptr = this->resolve_extern(nullptr, extern_name, false);

        emit_rpushes(std::forward<Args>(args)...);
        emit_flush_temp_regs();
        emit_call(target_ptr);
        emit_addi32(reg_esp, (sizeof...(args)* 4));
    }

    /// Emits a call to a __stdcall function.
    /// This emitter pushes all arguments and flushes temporary registers.
    template<typename... Args>
    void emit_stdcall(const char* extern_name, Args&&... args)
    {
        auto target_ptr = this->resolve_extern(nullptr, extern_name, false);

        emit_rpushes(std::forward<Args>(args)...);
        emit_flush_temp_regs();
        emit_call(target_ptr);
        // callee cleanup
    }

    /// Emits a call to a __thiscall function.
    /// This emitter pushes all arguments and flushes temporary registers.
    template<typename TEcx, typename... Args>
    void emit_thiscall(const char* extern_name, TEcx&& ecx, Args&&... args)
    {
        auto target_ptr = resolve_extern(nullptr, extern_name, false);

        emit_rpushes(std::forward<Args>(args)...);
        emit_flush_temp_regs();
        {
            auto reg_ecx = regalloc(Reg::Ecx);
            emit_movi32(reg_ecx, std::forward<TEcx>(ecx));
            emit_call(target_ptr);
        }
        // callee cleanup
    }

    /// Emits a call to a __fastcall function.
    /// This emitter pushes all arguments and flushes temporary registers.
    template<typename TEcx, typename TEdx, typename... Args>
    void emit_fastcall(const char* extern_name, TEcx&& ecx, TEdx&& edx, Args&&... args)
    {
        auto target_ptr = resolve_extern(nullptr, extern_name, false);

        emit_rpushes(std::forward<Args>(args)...);
        emit_flush_temp_regs();
        {
            auto reg_ecx = regalloc(Reg::Ecx);
            auto reg_edx = regalloc(Reg::Edx);
            emit_movi32(reg_edx, std::forward<TEdx>(edx));
            emit_movi32(reg_ecx, std::forward<TEcx>(ecx));
            emit_call(target_ptr);
        }
        // callee cleanup
    }

    template<typename TEcx>
    void emit_fastcall(const char* extern_name, TEcx&& ecx)
    {
        // fastcall with ecx only is equivalent to thiscall
        return emit_thiscall(extern_name, std::forward<TEcx>(ecx));
    }

    ///////////////////////////////////////////

private:
    /// Pushes `args...` in reverse order.
    template<typename Arg, typename... Rest>
    void emit_rpushes(Arg&& arg, Rest&&... rest)
    {
        this->emit_rpushes(std::forward<Rest>(rest)...);
        this->emit_push(std::forward<Arg>(arg));
    }

    /// Pushes `args...` in reverse order.
    void emit_rpushes()
    {
    }

private:
    void init_dasm();
    void free_dasm();

    void init_generators();
    void add_generator(int16_t opcode, opgen_func_t gen);

    optional<Reg> regalloc_base(Reg reg, bool force_alloc);
    optional<Reg> regalloc_base(int purposes);
    void regfree_base(Reg reg);

    friend IterData generate_code(const DecompiledData& data, IterData, CodeGeneratorIA32& codegen);

public:

    /// Tells movi32 to load effective address.
    template<typename T>
    struct AddressOf
    {
        // If this asserts, simply remove the check.
        // We're just checking, no reason at all.
        static_assert(std::is_same<T, const ArgVariant2&>::value, "");
        T value;
    };

    template<typename T>
    struct PointedBy
    {
        T value;
    };

    /// Tells movi32 to load effective address of value.
    auto lea(/* non const! */ ArgVariant2& x) -> AddressOf<const ArgVariant2&>
    {
        //                   ^ argument is non const to specify the object lifetime must be long enough
        return AddressOf<const ArgVariant2&> { x };
    }

    auto ptr(const ArgVariant2& x) -> PointedBy<const ArgVariant2&>
    {
        return PointedBy<const ArgVariant2&> { x };
    }

    auto ptr(RegGuard& x) -> PointedBy<RegGuard&>
    {
        return PointedBy<RegGuard&> { x };
    }

    auto ptr(uintptr_t x) -> PointedBy<uintptr_t>
    {
        return PointedBy<uintptr_t> { x };
    }





    /// The state of a x86 register.
    struct RegState
    {
    public:

    protected:
        friend struct CodeGeneratorIA32;
        friend struct CodeGeneratorIA32::RegGuard;

        RegState(Reg reg)
            : id(static_cast<uint8_t>(reg)), is_dirty(false), is_allocated(false)
        {}

        bool is_allocated : 1;  //< Is this register allocated by some one else?
        bool is_dirty     : 1;  //< Currently unused since we don't have a cache.
        uint8_t id;             //< Id of this register.      
    };

    /// Guard for a allocated register.
    /// When goes out of scope, frees the register.
    struct RegGuard
    {
    public:
        RegGuard(const RegGuard&) = delete;

        RegGuard(RegGuard&& rhs) :
            state(rhs.state), codegen(rhs.codegen)
        {
            rhs.state = nullptr;
        }

        ~RegGuard()
        {
            if(this->state)
                codegen->regfree_base(static_cast<Reg>(state->id));
        }

        RegState* operator->() {
            assert(state != nullptr);
            return state;
        }

        RegState& operator*() {
            assert(state != nullptr);
            return *state;
        }

        const RegState* operator->() const {
            assert(state != nullptr);
            return state;
        }
        const RegState& operator*() const {
            assert(state != nullptr);
            return *state;
        }

    protected:
        friend struct CodeGeneratorIA32;

        RegGuard() :
            state(nullptr)
        {}

        RegGuard(CodeGeneratorIA32& codegen, RegState& state) :
            state(&state), codegen(&codegen)
        {
        }

        RegGuard& operator=(const RegGuard& rhs) = delete;

        RegGuard& operator=(RegGuard&& rhs)
        {
            this->codegen = rhs.codegen;
            this->state = rhs.state;
            rhs.state = nullptr;
            return *this;
        }

    private:
        RegState*           state;
        CodeGeneratorIA32*  codegen;
    };

private:
    /// CRunningScript Context (EBP).
    RegGuard reg_ctx;
    /// Stack Pointer (ESP).
    RegGuard reg_esp;

    /// List of the state of all registers.
    RegState regstates[static_cast<size_t>(Reg::Max_)];

    /// Emitters.
    std::map<uint16_t, opgen_func_t> generators;
};



inline optional<int32_t> get_imm32(const EOAL&, CodeGeneratorIA32& codegen)
{
    return nullopt;
}

inline optional<int32_t> get_imm32(const DecompiledVar&, CodeGeneratorIA32& codegen)
{
    return nullopt;
}

inline optional<int32_t> get_imm32(const DecompiledVarArray&, CodeGeneratorIA32& codegen)
{
    return nullopt;
}

inline optional<int32_t> get_imm32(const DecompiledString&, CodeGeneratorIA32& codegen)
{
    return nullopt;
}

inline optional<int32_t> get_imm32(const int8_t& i8, CodeGeneratorIA32& codegen)
{
    return static_cast<int32_t>(i8);
}

inline optional<int32_t> get_imm32(const int16_t& i16, CodeGeneratorIA32& codegen)
{
    return static_cast<int32_t>(i16);
}

inline optional<int32_t> get_imm32(const int32_t& i32, CodeGeneratorIA32& codegen)
{
    return static_cast<int32_t>(i32);
}

inline optional<int32_t> get_imm32(const float& flt, CodeGeneratorIA32& codegen)
{
    // TODO floating point format static assert
    return reinterpret_cast<const int32_t&>(flt);
}

inline optional<int32_t> get_imm32(const ArgVariant2& varg, CodeGeneratorIA32& codegen)
{
    return visit_one(varg, [&](const auto& arg) { return ::get_imm32(arg, codegen); });
}

// Other overloads

inline optional<int32_t> get_imm32(const uint32_t& u32, CodeGeneratorIA32& codegen)
{
    return static_cast<int32_t>(u32);
}





template<typename TSrcAddr> inline
void CodeGeneratorIA32::emit_movi32(RegGuard& reg_dst, AddressOf<TSrcAddr> src)
{
    emit_lea(reg_dst, src.value);
}

template<typename TSrc> inline
void CodeGeneratorIA32::emit_movi32(const ArgVariant2& dst, TSrc& src)
{
    if(is<DecompiledVar>(dst))
    {
        emit_movi32(get<DecompiledVar>(dst), src);
    }
    else if(is<DecompiledVarArray>(dst))
    {
        // TODO
        throw DynarecError("Not imple yet");
    }
    else
    {
        throw DynarecUnexpectedValue(dst.which());
    }
}

/*
template<typename PTDst>
void CodeGeneratorIA32::emit_movu16_from_u32(PointedBy<PTDst> p_dst, const ArgVariant2& src)
{
    if(auto opt_imm32 = get_imm32(src, *this))
    {
        emit_movu16_from_u32(p_var_dst, *opt_imm32);
    }
    else
    {
        auto rx = this->regalloc(purposes_temp);
        emit_movi32(rx, src);
        emit_movu16_from_u32(p_var_dst, rx);
    }
}

template<typename TSrc>
void CodeGeneratorIA32::emit_movu16_from_u32(PointedBy<const ArgVariant2&> p_dst, TSrc& src)
{
    auto& dst_value = p_dst.value;

    if(auto opt_dst_imm32 = get_imm32(dst_value, *this))
    {
        emit_movu16_from_u32(*opt_dst_imm32, src);
    }
    else if(is<DecompiledVar>(dst))
    {
        emit_movu16_from_u32(get<DecompiledVar>(dst), src);
    }
    else if(is<DecompiledVarArray>(dst))
    {
        // TODO
        throw DynarecError("Not imple yet");
    }
    else
    {
        throw DynarecUnexpectedValue(dst.which());
    }
}*/

template<typename TDst, typename TSrc, typename TCount> inline
void CodeGeneratorIA32::emit_memcpy(TDst&& dst, TSrc&& src, TCount&& count_)
{
    // TODO optimization techniques
    // maybe forwarding to the optimized CRT would be a better idea?

    if(auto opt_count_imm32 = get_imm32(count_, *this))
    {
        size_t count = static_cast<size_t>(*opt_count_imm32);

        // we're flushing just because we'll use ECX/EDI/ESI, anything else we can do?
        this->emit_flush();
        {
            auto reg_ecx = this->regalloc(Reg::Ecx);
            auto reg_edi = this->regalloc(Reg::Edi);
            auto reg_esi = this->regalloc(Reg::Esi);

            emit_movi32(reg_esi, std::forward<TSrc>(src));
            emit_movi32(reg_edi, std::forward<TDst>(dst));

            if((count % 4) == 0)
            {
                emit_movi32(reg_ecx, count / 4);
                emit_rep(); emit_movsd();
            }
            else if((count % 2) == 0)
            {
                emit_movi32(reg_ecx, count / 2);
                emit_rep(); emit_movsw();
            }
            else
            {
                emit_movi32(reg_ecx, count);
                emit_rep(); emit_movsb();
            }
        }
    }
    else
    {
        auto& count = count_;

        // we're flushing just because we'll use ECX/EDI/ESI, anything else we can do?
        this->emit_flush();
        {
            auto reg_ecx = this->regalloc(Reg::Ecx);
            auto reg_edi = this->regalloc(Reg::Edi);
            auto reg_esi = this->regalloc(Reg::Esi);

            emit_movi32(reg_esi, std::forward<TSrc>(src));
            emit_movi32(reg_edi, std::forward<TDst>(dst));
            emit_movi32(reg_ecx, std::forward<TCount>(count));
            emit_rep(); emit_movsb();
        }
    }
}

/// Emits code equivalent to C++'s std::memset(dst, value, count), including
/// using only the first 8 bits of value.
template<typename TDst, typename TValue, typename TCount>
void CodeGeneratorIA32::emit_memset(TDst&& dst, TValue&& value, TCount&& count_)
{
    // TODO optimization techniques
    // * check if count == 1, if so do a simple mov
    // * maybe forwarding to the optimized CRT would be a better idea?
    // * MORE

    auto& count = count_;

    // we're flushing just because we'll use EAX/ECX/EDI, anything else we can do?
    this->emit_flush();
    {
        auto reg_eax = this->regalloc(Reg::Eax);
        auto reg_ecx = this->regalloc(Reg::Ecx);
        auto reg_edi = this->regalloc(Reg::Edi);

        emit_movi32(reg_edi, std::forward<TDst>(dst));
        emit_movi32_from_u8(reg_eax, std::forward<TValue>(value));
        emit_movi32(reg_ecx, std::forward<TCount>(count));
        emit_rep(); emit_stosb();
    }
}
