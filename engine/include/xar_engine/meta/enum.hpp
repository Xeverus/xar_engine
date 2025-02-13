#pragma


#define ENUM_TO_STRING(enum_type)   \
    namespace xar_engine::meta      \
    {                               \
        const char* const enum_to_string(enum_type enum_value);         \
        const char* const enum_to_short_string(enum_type enum_value);   \
    }                               \
    std::ostream& operator<<(std::ostream& stream, enum_type enum_value);
