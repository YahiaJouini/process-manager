#include "../../include/file_reader.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "../utils/is_numeric.cpp"

std::string FileReader::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string> FileReader::listDirectories(const std::string& path) {
    namespace fs = std::filesystem;
    std::vector<std::string> dirs;
    if (!fs::exists(path) || !fs::is_directory(path)) {
        std::invalid_argument("Path is not a directory or does not exist");
    }

    for (const auto& entry : fs::directory_iterator(path)) {
        // skip non directories
        if (!entry.is_directory()) continue;

        // directory with numeric names are processes
        const std::string pid_str = entry.path().filename();
        if (!is_numeric(pid_str)) continue;

        dirs.push_back(pid_str);
    }
    return dirs;
}
