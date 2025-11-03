#ifndef FORMAT_H
#define FORMAT_H

#include <string>

std::string truncate_string(const std::string&, size_t);
std::string format_memory(float);
std::string format_cpu(float);
std::string get_state_name(const std::string&);

#endif
