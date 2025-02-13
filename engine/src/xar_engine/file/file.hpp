#pragma once

#include <filesystem>
#include <vector>


namespace xar_engine::file
{
    class File
    {
    public:
        static std::vector<char> read_binary_file(const std::filesystem::path& filepath);
    };
}