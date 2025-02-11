#pragma

#include <xar_engine/meta/for_each.hpp>


#define ENUM_TO_STRING(enum_type)   \
    namespace xar_engine::meta      \
    {                               \
        std::string enum_to_string(enum_type enum_value); \
        std::ostream& operator<<(std::ostream& stream, enum_type enum_value); \
    }


#define ENUM_TO_STRING_CASE(enum_value) \
    case enum_value: return #enum_value;


#define ENUM_TO_STRING_IMPL(enum_type, ...)   \
    namespace xar_engine::meta           \
    {                                    \
        std::string enum_to_string(const enum_type enum_value) \
        {                                \
            switch (enum_value)          \
            {                            \
                FOR_EACH(ENUM_TO_STRING_CASE, __VA_ARGS__)  \
            }                            \
                                         \
            XAR_THROW(                   \
                error::XarException,     \
                #enum_type " enum value doesn't define value '{}'", \
                static_cast<int>(enum_value));                      \
        }                                \
                                         \
        std::ostream& operator<<(        \
            std::ostream& stream,        \
            const enum_type enum_value)  \
        {                                \
            if (stream.good())           \
            {                            \
                stream << enum_to_string(enum_value);          \
            }                            \
                                         \
            return stream;               \
        }                                \
    }
