#pragma once
#include <dirent.h>
#include <unistd.h>

struct linux_dirent64 {
    ino64_t        d_ino;
    off64_t        d_off;
    unsigned short d_reclen;
    unsigned char  d_type;  
    char           d_name[];
};