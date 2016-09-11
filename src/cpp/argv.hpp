///
/// Argv Parser so that we can have GCC-like argument options. 
///
#pragma once
#include <cstddef>
#include <climits>
#include <stdexcept>

/// Thrown when the option formating is wrong in a `optget` or `optflag`.
struct invalid_opt : std::runtime_error
{
    explicit invalid_opt(const std::string& err) :
        std::runtime_error(err)
    {}
};

///
/// Checks if current position of `argv` is `shortopt` or `longopt`.
///
/// If it is, returns a non null pointer, which points to the argument to the option if `num_values` is `1`.
/// It also increments `argv` accordingly, so that its past the argument just read.
///
/// Otherwise, returns nullptr and argv is not modified.
///
inline char* optget(char**& argv, const char* shortopt, const char* longopt, size_t num_values)
{
    assert(*argv != nullptr);
    assert(shortopt == 0 || strlen(shortopt) == 2);
    assert(num_values <= 1);

    char *eq_pos = nullptr, *eq_pos_beg;

    if(num_values)
    {
        eq_pos = strchr(*argv, '=');
        eq_pos_beg = eq_pos? eq_pos + 1 : 0;
    }

    if(longopt && !strncmp(*argv, longopt, eq_pos? (eq_pos - *argv) : INT_MAX))
    {
        ++argv;
        if(num_values)
        {
            char** p = eq_pos? &eq_pos_beg : argv;
            if(*p == nullptr || **p == '\0')
            {
                --argv;
                throw invalid_opt(fmt::format("argument to '{}' is missing (expected {} value).", longopt, num_values));
            }
            if(p == argv)
                ++argv;
            return *p;
        }
        else
        {
            return *(argv-1); // dummy pointer
        }
    }

    if(shortopt && !strncmp(*argv, shortopt, 2))
    {
        ++argv;
        if(num_values)
        {
            char* arg2 = (*(argv - 1)) + 2;
            char** p = *arg2? &arg2 : argv;
            if(*p == nullptr || **p == '\0')
            {
                --argv;
                throw invalid_opt(fmt::format("argument to '{:2}' is missing (expected {} value).", shortopt, num_values));
            }
            if(p == argv)
                ++argv;
            return *p;
        }
        else
        {
            return *(argv-1); // dummy pointer
        }
    }

    return nullptr;
}

///
/// Checks if current position of `argv` is the flag `-fflagname`.
///
/// Returns a non null pointer on success and increments `argv` accordingly.
/// On failure, `argv` is not modified.
///
/// If `flag` is not a null pointer, checks both for `-fno-flagname` and `-fflagname`, then returns
/// `false` or `true` for those respectively.
///
/// Otherwise, if `flag` is a null pointer, checks only for `-fflagname`.
///
inline char* optflag(char**& argv, const char* flagname, bool* flag)
{
    assert(*argv != nullptr);

    char temp_c_f[4], temp_c_fno[8];
    const char* c_f   = "-f";
    const char* c_fno = "-fno-";

    // in case one wants to use e.g. -m instead of -f
    if(flagname[0] == '-')
    {
        if(flagname[1] != 'f')
        {
            assert(flagname[1] != 0);

            strcpy(temp_c_f, c_f);
            strcpy(temp_c_fno, c_fno);
            temp_c_f[1] = flagname[1];
            temp_c_fno[1] = flagname[1];

            c_f = temp_c_f;
            c_fno = temp_c_fno;
        }

        flagname += 2;
    }

    if(flag && !strncmp(*argv, c_fno, 5))
    {
        *argv += 5;
        if(char* val = optget(argv, nullptr, flagname, 0))
        {
            *flag = false;
            return val;
        }
        else
        {
            *argv -= 5;
        }
    }

    if(!strncmp(*argv, c_f, 2))
    {
        *argv += 2;
        if(char* val = optget(argv, nullptr, flagname, 0))
        {
            if(flag) *flag = true;
            return val;
        }
        else
        {
            *argv -= 2;
        }
    }

    return nullptr;
}

///
/// Checks if current position of `argv` is `longopt`.
///
/// If it is, returns a non null pointer, and `out` is modified to contain the integer value on the option.
/// It also increments `argv` accordingly, so that its past the argument just read.
///
/// Otherwise, returns nullptr and argv nor out is not modified.
///
template<typename T>
inline const char* optint(char**& argv, const char* longopt, T* out, int base = 10)
{
    if(const char* value = optget(argv, nullptr, longopt, 1))
    {
        try
        {
            long long integer = std::stoll(value, 0, base);
            if(integer >= std::numeric_limits<T>::min() && integer <= std::numeric_limits<T>::max())
            {
                *out = static_cast<T>(integer);
                return value;
            }
            else
            {
                throw invalid_opt(fmt::format("argument '{}' value is too little or too big: '{}'.", longopt, value));
            }
        }
        catch(const std::exception&)
        {
            throw invalid_opt(fmt::format("argument '{}' expectes a integer, got '{}'.", longopt, value));
        }
    }
    return nullptr;
}
