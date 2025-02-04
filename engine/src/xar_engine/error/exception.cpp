#include <xar_engine/error/exception.hpp>

namespace xar_engine::error
{
    XarException::XarException(std::string&& message)
        : _message(std::move(message))
    {
    }

    const char* XarException::what() const noexcept
    {
        return _message.c_str();
    }
}