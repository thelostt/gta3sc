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




    ///////////////////////////////////////////
    // Instruction Emitter
    ///////////////////////////////////////////

    // See also emit_flush
    // See also emit_flush_temp_regs

    /// Emits a call into the specified absolute target.
    ///
    /// \warning This simply emits a call instruction, it doesn't flush any temp register or cleanup the stack.
    /// Please see `emit_ccall`, `emit_stdcall` and `emit_thiscall` for *better* alternatives.
    void emit_call(int32_t target_ptr);

    // x86 PUSH
    void emit_pushi32(int32_t imm32);
    void emit_pushi32(RegGuard& reg);
    void emit_pushi32(const DecompiledVar& var);
    void emit_pushi32(const ArgVariant2& varg);

    // x86 MOV
    void emit_movi32(const DecompiledVar& dst, RegGuard& reg_src);
    void emit_movi32(RegGuard& reg_dst, int32_t imm32);
    void emit_movi32(RegGuard& reg_dst, const DecompiledVar& src);
    void emit_movi32(RegGuard& reg_dst, const ArgVariant2& src);
    void emit_movi32(const ArgVariant2& dst, const ArgVariant2& src);
    void emit_movi32(const DecompiledVar& var_dst, int32_t imm32);
    void emit_movi32(const DecompiledVar& var_dst, const ArgVariant2& src);

    // x86 ADD
    void emit_addi32(RegGuard& reg_dst, int32_t imm32);
    void emit_addi32(const ArgVariant2& dst, const ArgVariant2& src);
    void emit_addi32(const DecompiledVar& var_dst, const ArgVariant2& src);
    void emit_addi32(const DecompiledVar& var_dst, int32_t imm32);
    void emit_addi32(const DecompiledVar& dst, RegGuard& reg_src);

    // x86 SUB
    void emit_subi32(RegGuard& reg_dst, int32_t imm32);


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
    template<typename... Args>
    void emit_thiscall(const char* extern_name, void* this_ptr, Args&&... args)
    {
        auto target_ptr = resolve_extern(nullptr, extern_name, false);

        emit_rpushes(std::forward<Args>(args)...);
        emit_flush_temp_regs();
        {
            auto reg_ecx = regalloc(Reg::Ecx);
            emit_movi32(reg_ecx, static_cast<int32_t>(this_ptr));
            emit_call(target_ptr);
        }
        // callee cleanup
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
