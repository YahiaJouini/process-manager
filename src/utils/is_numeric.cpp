#include <string>

bool is_numeric(const std::string& s) {
    for (char c : s) {
        if (!isdigit(c)) return false;
    }

    return true;
}
