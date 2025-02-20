#include <xar_engine/file/file.hpp>

#include <fstream>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::file
{
    std::vector<char> read_binary_file(const std::filesystem::path& filepath)
    {
        std::ifstream file(filepath.c_str(), std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            XAR_THROW(
                error::XarException,
                "Failed to read binary file {}",
                filepath.string());
        }

        const auto file_bytesize = file.tellg();
        std::vector<char> buffer(file_bytesize);

        file.seekg(0);
        file.read(
            buffer.data(),
            file_bytesize);

        return buffer;
    }
}