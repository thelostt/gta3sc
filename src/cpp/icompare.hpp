/// Case-Insensitive Compare
///
#pragma once
#include <string>
#include <cstring>

#if defined(_WIN32)
inline int strcasecmp(const char* a, const char* b)
{
    return _stricmp(a, b);
}
#elif defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
    // Already has strcasecmp
#else
#   error
#endif

/// std::less<void> but case insensitive
struct iless
{
    using is_transparent = int;

    bool operator()(const std::string& left, const std::string& right) const
    {
        return this->operator()(left.c_str(), right.c_str());
    }

    bool operator()(const char* left, const std::string& right) const
    {
        return this->operator()(left, right.c_str());
    }

    bool operator()(const std::string& left, const char* right) const
    {
        return this->operator()(left.c_str(), right);
    }

    bool operator()(const char* left, const char* right) const
    {
        return (strcasecmp(left, right) < 0);
    }
};

/// std::equal_to<void> but case insensitive
struct iequal_to
{
    using is_transparent = int;

    bool operator()(const std::string& left, const std::string& right) const
    {
        return this->operator()(left.c_str(), right.c_str());
    }

    bool operator()(const char* left, const std::string& right) const
    {
        return this->operator()(left, right.c_str());
    }

    bool operator()(const std::string& left, const char* right) const
    {
        return this->operator()(left.c_str(), right);
    }

    bool operator()(const char* left, const char* right) const
    {
        return (strcasecmp(left, right) == 0);
    }
};
