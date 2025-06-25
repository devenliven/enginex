#ifndef UTILITIES_FILE_H_
#define UTILITIES_FILE_H_

#include "utilities/logger.h"

#include <string>
#include <filesystem>
#include <optional>
#include <fstream>

class FileSystem
{
  public:
    FileSystem()  = default;
    ~FileSystem() = default;

    static std::optional<std::string> readFileToString(const std::string& filePath)
    {
        if (!std::filesystem::exists(filePath)) {
            LOG_DEBUG("FileSystem: {} does not exist.", filePath);
            return std::nullopt;
        }

        if (!std::filesystem::is_regular_file(filePath)) {
            LOG_DEBUG("FileSystem: {} is a directory, not a folder", filePath);
            return std::nullopt;
        }

        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            LOG_DEBUG("FileSystem: {} is not readable.!", filePath);
            return std::nullopt;
        }

        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::string content(size, '\0');
        file.read(content.data(), size);
        return content;
    }
};

#endif // UTILITIES_FILE_H_