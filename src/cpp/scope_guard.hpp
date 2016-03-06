/// Scope Guard - Execute something at the end of the scope
///
#pragma once
#include <type_traits>
#include <utility>

/// Ensures something gets run at the end of a scope.
template<typename Functor>
class scope_guard final
{
public:
    static_assert(std::is_nothrow_move_constructible<Functor>::value, "Functor must be NoThrow MoveConstructible");

    scope_guard(Functor f)
        : fun(std::move(f)), dismissed(false)
    {}

    ~scope_guard() noexcept
    {
        this->execute();
    }

    scope_guard(const scope_guard&) = delete;
    scope_guard(scope_guard&& rhs) noexcept
        : dismissed(rhs.dismissed), fun(std::move(rhs.fun))
    {
        rhs.dismiss();
    }

    scope_guard& operator=(scope_guard const&) = delete;
    scope_guard& operator=(scope_guard&&) = delete;

    /// Don't call the guard at the end of the scope anymore.
    void dismiss() noexcept
    {
        this->dismissed = true;
    }

    // Executes the guard now instead of at the end of the scope.
    void execute() noexcept
    {
        if(!this->dismissed)
        {
            this->fun();
            this->dismiss();
        }
    }

private:
    Functor fun;
    bool    dismissed;
};

/// Executes the functor `f` at the end of the outer scope.
template<typename Functor>
inline auto make_scope_guard(Functor f) -> scope_guard<Functor>
{
    return scope_guard<Functor>(std::move(f));
}
