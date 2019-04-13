#pragma once

#include <vector>
#include <unordered_map>

class Program {
public:
    Program();
    ~Program();
    Program(const std::string& name, const std::vector<std::string>& args, const std::unordered_map<std::string, std::string> &envs);
    void print_info ();
    char* get_name();
    char** get_envs();
    char** get_args();
private:
    char* name;
    char** args;
    char** envs;
};