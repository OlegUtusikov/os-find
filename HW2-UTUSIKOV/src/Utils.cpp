#include <sstream>
#include <iostream>
#include <vector>
#include <string.h>
#include <algorithm>
#include <exception>
#include "./../headers/Utils.h"



std::vector<std::string> Utils::parse(int argc, char** argv) {
    std::vector<std::string> res;
    for(size_t i = 0; i < (size_t)argc; i++) {
        res.emplace_back(argv[i]);
    }
    return res;
}

std::string Utils::create_path(const std::string &parent_path, const std::string &child) {
    std::string res;
    std::stringstream ss;
    ss << (parent_path[parent_path.size() - 1] == '/' ? parent_path : parent_path + "/");
    ss << child;
    std::string tmp;
    while(std::getline(ss, tmp, '/')) {
        res += tmp + "/"; 
    }
    return res.substr(0, res.size() - 1);
}

bool Utils::is_directory(std::string &path) {
    return S_ISDIR(get_stat(path).st_mode);
}

struct stat Utils::get_stat(const std::string& path) {
    struct stat buffer{};
    if (stat(path.c_str(), &buffer) == -1) {
        std::error_code ec(errno, std::system_category());
        throw std::system_error(ec, "Failed get a information about file " +  path + ". Cause: " + strerror(errno));
    }
    return buffer;
}

std::string Utils::get_name(const std::string &path) {
    std::string tmp = path;
    if (tmp.empty()) {
        return "";
    }
    if (tmp[tmp.size() - 1] == '/') {
        tmp = tmp.substr(0, tmp.size() - 1);
    }
    if (tmp.empty()) {
        return "";
    }
    std::stringstream ss;
    size_t ind = tmp.size() - 1;
    while(ind >= 0 && tmp[ind] != '/') {
        ss << tmp[ind--];
    }
    std::string res;
    ss >> res;
    std::reverse(res.begin(), res.end());
    return res;
}

bool Utils::isNumber(const std::string &s) {
    for(auto si: s) {
        if (!isdigit(si)) {
            return false;
        }
    }
    return true;
}
