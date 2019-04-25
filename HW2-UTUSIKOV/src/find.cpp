#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <queue>
#include <sstream>
#include <map>
#include <algorithm>
#include <unordered_set>

#include "./../headers/Utils.h"
#include "./../headers/Dirent.h"
#include "./../headers/Enviroment.h"
#include "./../headers/Program.h"

int find_dfs(const std::string& path, std::unordered_set<std::string> &paths) {
    if (paths.count(path) > 0) {
        return 0;
    }
    paths.insert(path);
    const int BUF_SIZE = 64 * 1024 ;
    char* buf = new char[BUF_SIZE];
    int fd = open(path.c_str(), O_RDONLY | O_DIRECTORY);
    if (fd == -1) {
        std::error_code ec(errno, std::system_category());
        close(fd);
        delete[] buf;
        throw std::system_error(ec, "Couldn't open a directory by path: " + path);
    }
    int count_of_bytes = 0;
    while(true) {
        count_of_bytes = static_cast<int>(syscall(SYS_getdents64, fd, buf, BUF_SIZE));
        if (count_of_bytes == -1) {
            std::error_code ec(errno, std::system_category());
            close(fd);
            delete[] buf;
            throw std::system_error(ec, "Error in reading by " +  path + " (cause: " + strerror(errno) + ").");
        }
        if (count_of_bytes == 0) {
            break;
        }
        for(int pos = 0; pos < count_of_bytes;) {
            auto *d = (struct linux_dirent64*)(buf + pos);
            std::string name(d->d_name);
            if (name == "." || name == "..") {
                pos += d->d_reclen;
                continue;
            }
            std::string new_path = Utils::create_path(path, name);
            if (Utils::is_directory(new_path)) {
                find_dfs(new_path + "/", paths);
            } else {
                paths.insert(new_path);
            }
            pos += d->d_reclen;
        }
    }
    close(fd);
    delete[] buf;
    return 0;
}


int execute_program(std::vector<std::string> args, Enviroment enviroment) {
    pid_t pid = fork();
    int code = -1;
    if (pid == -1) {
        std::error_code ec(errno, std::system_category());
        throw std::system_error(ec, "Cann't create a new process. \"fork\" failed.");
    } else if (pid == 0) {
        Program program(args[0], args, enviroment.get_variables());
        if (execve(program.get_name(), program.get_args(), program.get_envs()) < 0) {
            std::error_code ec(errno, std::system_category());
            throw std::system_error(ec, "Executing of program was failed");
        }
    } else if (pid > 0) {
        int status = 0;
        pid_t p = waitpid(pid, &status, 0);
        if (p > 0) {
            if (WIFEXITED(status)) {
                code = WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                std::error_code ec(errno, std::system_category());
                throw std::system_error(ec, "Program was terminated by signal with code " + WTERMSIG(status));
            } else if (WCOREDUMP(status)) {
                std::error_code ec(errno, std::system_category());
                throw std::system_error(ec, "Program generated a error in core of system.");
            } else {
                std::error_code ec(errno, std::system_category());
                throw std::system_error(ec, "Process was stopped by signal with code " + WSTOPSIG(status));
            }
        } else {
            std::error_code ec(errno, std::system_category());
            throw std::system_error(ec, "Program was terminated right away.");
        }

    }
    return code;
}

int find (const std::string& path, const std::vector<std::string>& args, char** env) {
    std::unordered_set<std::string> paths;
    find_dfs(path, paths);
    if (!args.empty()) {
        Enviroment env_vars;
        for(size_t i = 0; env[i] != nullptr; i++) {
            env_vars.add_variable(env[i]);
        }
        for(size_t i = 0; i + 1 < args.size(); i += 2) {
            std::unordered_set<std::string> tmp;
            if (args[i] == "-inum") {
                if (!Utils::isNumber(args[i + 1])) {
                    std::error_code ec(errno, std::system_category());
                    throw std::system_error(ec, "Argument for -inum must be a number.");
                }
                auto num = static_cast<ino64_t>(atoi(args[i + 1].c_str()));
                for(const auto &pi : paths) {
                    try {
                        if (Utils::get_stat(pi).st_ino == num) {
                            tmp.insert(pi);
                        }
                    } catch (std::system_error &e) {
                        throw e;
                    }
                }
            } else if (args[i] == "-name") {
                for(const auto &pi : paths) {
                    if (Utils::get_name(pi) == args[i + 1]) {
                        tmp.insert(pi);
                    }
                }
            } else if (args[i] == "-size") {
                char c = args[i + 1][0];
                std::string tmp_s = args[i + 1].substr(1, args[i + 1].size() - 1);
                if (!Utils::isNumber(tmp_s)) {
                    std::error_code ec(errno, std::system_category());
                    throw std::system_error(ec, "Argument for -size must be a number.");
                }
                auto size = static_cast<size_t>(atoi(tmp_s.c_str()));
                switch (c) {
                    case '-':
                        for(const auto &pi : paths) {
                            try {
                                if (Utils::get_stat(pi).st_size < size) {
                                    tmp.insert(pi);
                                }
                            } catch (std::system_error &e) {
                                throw e;
                            }
                        }
                        break;
                    case '+':
                        for(const auto &pi : paths) {
                            try {
                                if (Utils::get_stat(pi).st_size > size) {
                                    tmp.insert(pi);
                                }
                            } catch (std::system_error &e) {
                                throw e;
                            }
                        }
                        break;
                    case '=':
                        for(const auto &pi : paths) {
                            try {
                                if (Utils::get_stat(pi).st_size == size) {
                                    tmp.insert(pi);
                                }
                            } catch (std::system_error &e) {
                                throw e;
                            }
                        }
                        break;
                    default:
                        throw 1;
                }
            } else if (args[i] == "-nlinks") {
                int num = atoi(args[i + 1].c_str());
                if (!Utils::isNumber(args[i + 1])) {
                    std::error_code ec(errno, std::system_category());
                    throw std::system_error(ec, "Argument for -nlinks must be a number.");
                }
                try {
                    for(const auto &pi  : paths) {
                        if (Utils::get_stat(pi).st_nlink == num) {
                            tmp.insert(pi);
                        }
                    }
                } catch (std::system_error &e) {
                    throw e;
                }
            } else if (args[i] == "-exec") {
                std::vector<std::string> params;
                params.push_back(args[i + 1]);
                for(const auto &pi : paths) {
                    std::cout << pi << std::endl;
                    params.push_back(pi);
                }
                if (params.size() == 1) {
                    std::cerr << "Count of paths for exec = 0." << std::endl;
                } else {
                    try {
                        execute_program(params, env_vars);
                    } catch (std::system_error &e) {
                        throw e;
                    }
                }
                // I am not sure, that we mast clear a tmp after -exec, in the discription this theme doesn't discribe.
                //tmp.clear();
            } else {
                std::error_code ec(errno, std::system_category());
                throw std::system_error(ec, "Unknown key!");
            }
            paths = tmp;
        }
    }
    for(const auto &pi : paths) {
        std::cout << pi << std::endl;
    }
    paths.clear();
    return 0;
}

int main(int argc, char** argv, char** env) {
    std::vector<std::string> args = Utils::parse(argc, argv);
    args.erase(args.begin());
    std::string path;
    if (args.empty()) {
        path = "./";
    } else {
        path = args[0];
        args.erase(args.begin());
    }
    try {
        find(path, args, env);
    } catch (std::system_error &e) {
        std::cerr << "Find was terminated cause: " << e.what() << std::endl;
    }
    exit(EXIT_SUCCESS);
}