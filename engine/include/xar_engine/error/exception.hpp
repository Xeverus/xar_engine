#pragma once

#include <exception>
#include <string>


namespace xar_engine::error
{
    class XarException : public std::exception
    {
    public:
        explicit XarException(std::string&& message);

        [[nodiscard]]
        const char* what() const noexcept override;

    private:
        std::string _message;
    };
}
