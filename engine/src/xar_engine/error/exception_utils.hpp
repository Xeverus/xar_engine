#pragma once

#include <cstdint>

#include <fmt/core.h>

#include <xar_engine/error/exception.hpp>


namespace xar_engine::error
{

#define XAR_FORMAT_EXCEPTION_MESSAGE(reason, message, ...) \
    fmt::format(FMT_STRING("Reason: {}\nFile: {}\nLine: {}\nMessage: " message), reason, __FILE__, __LINE__, ##__VA_ARGS__)


#define XAR_THROW_IF(condition, ExceptionType, message, ...)                                     \
    do                                                                                           \
    {                                                                                            \
      if ((condition))                                                                           \
      {                                                                                          \
        throw ExceptionType{ XAR_FORMAT_EXCEPTION_MESSAGE(#condition, message, ##__VA_ARGS__) }; \
      }                                                                                          \
    }                                                                                            \
    while(false)


#define XAR_THROW(ExceptionType, message, ...) \
    XAR_THROW_IF(true, ExceptionType, message, ##__VA_ARGS__)


#define XAR_OBJECT_ID(this_pointer) \
    fmt::format("{:#10x}", reinterpret_cast<std::uint64_t>(this_pointer))

}