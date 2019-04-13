project(Find)

set(CMAKE_CXX_STANDARD 14)
add_executable(Find ./headers/Dirent.h ./headers/Utils.h ./src/Utils.cpp ./headers/Enviroment.h ./src/Enviroment.cpp ./headers/Program.h ./src/Program.cpp ./src/find.cpp)