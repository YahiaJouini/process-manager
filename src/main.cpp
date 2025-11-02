#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

bool is_numeric(const std::string& s) {
    for (char c : s) {
        if (!isdigit(c)) return false;
    }

    return true;
}

int main() {
    const fs::path proc_path = "/proc";

    if (!fs::exists(proc_path) || !fs::is_directory(proc_path)) {
        std::cout << "Processes directory does not exist" << std::endl;
    }

    for (const auto& entry : fs::directory_iterator(proc_path)) {
        // skip non directories
        if (!entry.is_directory()) continue;

        // directory with numeric names are processes
        const std::string pid_str = entry.path().filename();
        if (!is_numeric(pid_str)) continue;

        // path to process name
        const fs::path comm_path = proc_path.string() + "/" + pid_str + "/comm";

        // fstream class takes path in constructor and opens it for reading
        std::ifstream file(comm_path);
        std::string process_name;

        if (file.is_open()) {
            std::getline(file, process_name);
            file.close();
            std::cout << "PID: " << pid_str << "  |  Name: " << process_name
                      << std::endl;
        }
    }

    return 0;
}
