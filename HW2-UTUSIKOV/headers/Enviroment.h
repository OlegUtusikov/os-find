#pragma once
#include <unordered_map>
#include <vector>

class Enviroment {
public:
    Enviroment();
    void add_variable(const std::string& var);
    void remove_variable(const std::string& var);
    std::string get_var(const std::string& key);
    std::unordered_map<std::string, std::string> get_variables();
    ~Enviroment();
private:
    std::unordered_map<std::string, std::string> variables;
};