#include <string>
#include <vector>
#ifndef FILE_READER_H
#define FILE_READER_H

class FileReader {
   public:
    std::string read_file(const std::string& path);
    std::vector<std::string> list_directories(const std::string& path);
};

#endif
