#pragma once

#include <fmt/core.h>

#include <xar_engine/error/exception.hpp>

namespace xar_engine::error
{

#define XAR_FORMAT_EXCEPTION_MESSAGE(reason, message, ...) \
    fmt::format("Reason: {}\nFile: {}\nLine: {}\nMessage: " message, reason, __FILE__, __LINE__, ##__VA_ARGS__)

#define XAR_THROW_IF(condition, ExceptionType, message, ...)                                                 \
    if ((condition))                                                                                         \
    {                                                                                                        \
      throw ExceptionType{ XAR_FORMAT_EXCEPTION_MESSAGE(#condition, message, ##__VA_ARGS__) };   \
    }

}