#include <vector>
#include "Dirent.h"
#include <string>

#include <sys/types.h>
#include <sys/stat.h>

class Utils {
    public:
        static std::vector<std::string> parse(int argc, char** argv);

        static std::string create_path(const std::string &parent_path, const std::string &child);

        static bool is_directory(std::string &path);

        static struct stat get_stat(const std::string &path);

        static std::string get_name(const std::string &path);

        static bool isNumber(const std::string &s);

    private:
};

