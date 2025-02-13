#pragma

#include <cstring>
#include <ostream>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/meta/enum.hpp>
#include <xar_engine/meta/for_each.hpp>


#define ENUM_TO_STRING_CASE(enum_value) \
    case enum_value: return #enum_value;


#define ENUM_TO_STRING_IMPL(enum_type, ...)   \
    namespace xar_engine::meta           \
    {                                    \
        const char* const enum_to_string(const enum_type enum_value) \
        {                                \
            switch (enum_value)          \
            {                            \
                FOR_EACH(ENUM_TO_STRING_CASE, __VA_ARGS__)  \
            }                            \
                                         \
            XAR_THROW(                   \
                ::xar_engine::error::XarException, \
                #enum_type " enum value doesn't define value '{}'", \
                static_cast<int>(enum_value));                      \
        }                                \
                                         \
        const char* const enum_to_short_string(const enum_type enum_value) \
        {                                \
            const auto* const enum_value_string = enum_to_string(enum_value); \
            return 1 + strrchr(enum_value_string, ':'); \
        }                                \
    }                                    \
                                         \
    std::ostream& operator<<(            \
        std::ostream& stream,            \
        const enum_type enum_value)      \
    {                                    \
        if (stream.good())               \
        {                                \
            stream << xar_engine::meta::enum_to_string(enum_value); \
        }                                \
                                         \
        return stream;                   \
    }
