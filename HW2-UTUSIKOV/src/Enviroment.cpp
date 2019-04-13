#include "./../headers/Enviroment.h"
#include <iostream>

Enviroment::Enviroment() {

}

Enviroment::~Enviroment() = default;

void Enviroment::add_variable(const std::string& var) {
    size_t ind = var.find('=');
    std::string key = var.substr(0, ind);
    std::string value = var.substr(ind + 1, var.size() - ind);
    if (key == "PATH" && value.find("$PATH") != std::string::npos) {
        size_t ind = value.find("$PATH");
        if (ind == 0) {
            variables[key] = variables[key] + value.substr(5, value.size() - 5);
        } else if (ind == (value.size() - 5)) {
            variables[key] = value.substr(0, value.size() - 5) + variables[key];
        } else {
            std::string p1 = value.substr(0, variables[key].size() > 0 ? ind : ind - 1);
            std::string p2 = value.substr(ind + 5, value.size() - p1.size() - 5);
            variables[key] = p1 + variables[key] + p2;
        }
    } else {
        variables[key] = value;
    } 
}

std::string Enviroment::get_var(const std::string& key) {
    if (variables.find(key) == variables.end()) {
        return "";
    } else {
        return variables[key];
    }
}

std::unordered_map<std::string, std::string> Enviroment::get_variables() {
    return variables;
}

void Enviroment::remove_variable(const std::string& var) {
    variables.erase(var);
}
