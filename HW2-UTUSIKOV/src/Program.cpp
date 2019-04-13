#include <iostream>
#include <string.h>
#include <vector>
#include "./../headers/Program.h"

Program::Program() {
    name = nullptr;
    args = nullptr;
    envs = nullptr;
}

Program::Program(const std::string& name, 
                const std::vector<std::string>& args, 
                const std::unordered_map<std::string, std::string> &envs){
    this->name = new char[name.size() + 1];
    strcpy(this->name, name.c_str());
    //std::cout << "INFO: 1 " << this->name << std::endl;
    this->args = new char*[args.size() + 1];
    this->args[args.size()] = nullptr;
    for(size_t i = 0; i < args.size(); i++) {
        this->args[i] = new char[args[i].size() + 1];
        strcpy(this->args[i], args[i].c_str());
    }
    //std::cout << "INFO: 2" << std::endl;
    this->envs = new char*[envs.size() + 1];
    this->envs[envs.size()] = nullptr;
    size_t ind = 0;
    for(const auto &envi : envs)  {
        std::string str(envi.first + "=" + envi.second);
        this->envs[ind] = new char[str.size() + 1];
        strcpy(this->envs[ind], str.c_str());
        ind++;
    }
    //std::cout << "INFO: 3 " << std::endl;
}

Program::~Program() {
    delete[] name;
    for(size_t i = 0; args[i] != nullptr; i++) {
        delete[] args[i];
    }
    for(size_t i = 0; envs[i] != nullptr; i++) {
        delete[] envs[i];
    }
    delete[] args;
    delete[] envs;
}

void Program::print_info() {
    std::cout << "++++++++++++++++++++++++++++" << std::endl;
    std::cout << "Name of program is " << name << std::endl;
    std:: cout << "Args:" << std::endl;
    for(size_t i = 0; args[i] != nullptr; i++) {
        std:: cout << args[i] << " ";
    }
    std::cout << std::endl << "Envs: " << std::endl;
    for(size_t i = 0; envs[i] != nullptr; i++) {
        std::cout << envs[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "++++++++++++++++++++++++++++" << std::endl;
}

char* Program::get_name() {
    return name;
}

char** Program::get_envs() {
    return envs;
}

char** Program::get_args() {
    return args;
}
