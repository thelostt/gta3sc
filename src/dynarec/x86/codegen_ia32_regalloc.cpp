#include "../../stdinc.h"
#include "codegen_ia32.hpp"

//
// Our register allocator is very very basic, it's not caching stuff yet.
//


auto CodeGeneratorIA32::regalloc_base(Reg reg, bool force_alloc) -> optional<Reg>
{
    auto id = static_cast<size_t>(reg);
    if(this->regstates[id].is_allocated == false)
    {
        Expects(regstates[id].is_dirty == false);
        regstates[id].is_allocated = true;
        return static_cast<Reg>(id);
    }

    if(force_alloc)
    {
        throw DynarecError("Failed to allocate register {}", id);
    }

    return nullopt;
}

auto CodeGeneratorIA32::regalloc_base(int purposes) -> optional<Reg>
{
    auto xalloc_try = [this](Reg reg) -> optional<Reg>
    {
        return this->regalloc_base(reg, false);
    };

    if(purposes == purposes_temp)
    {
        // Try allocating from EAX (most generic) to EDI (least generic).
        for(size_t i = 0; i < static_cast<size_t>(Reg::Max_); ++i)
        {
            auto reg = static_cast<Reg>(i);
            if(xalloc_try(reg))
                return reg;
        }
    }
    else if(purposes == purposes_store)
    {
        // Try allocating only from the non-generic registers.
        for(size_t k = static_cast<size_t>(Reg::Ebx); k < static_cast<size_t>(Reg::Max_); --k)
        {
            auto reg = static_cast<Reg>(k);
            if(xalloc_try(reg))
                return reg;
        }
    }
    else
    {
        Unreachable();
    }

    return nullopt;
}

void CodeGeneratorIA32::regfree_base(Reg reg)
{
    auto id = static_cast<size_t>(reg);
    Expects(regstates[id].is_allocated);
    regstates[id].is_allocated = false;
}

auto CodeGeneratorIA32::regalloc(int purposes) -> RegGuard
{
    if(auto opt_reg = regalloc_base(purposes))
    {
        auto id = static_cast<size_t>(*opt_reg);
        return RegGuard(*this, this->regstates[id]);
    }
    else
    {
        throw DynarecError("Register allocation failed! This is a bug!!!");
    }
}

auto CodeGeneratorIA32::regalloc(Reg reg) -> RegGuard
{
    auto _  = *regalloc_base(reg, true);
    auto id = static_cast<size_t>(reg);
    return RegGuard(*this, this->regstates[id]);
}
