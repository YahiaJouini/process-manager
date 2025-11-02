#include <string>
#include <vector>
#ifndef FILE_READER_H
#define FILE_READER_H

class FileReader {
   public:
    std::string readFile(const std::string& path);
    std::vector<std::string> listDirectories(const std::string& path);
};

#endif
