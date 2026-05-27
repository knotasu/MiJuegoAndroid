cmake_minimum_required(VERSION 3.22)
project("mygame")

set(CMAKE_CXX_STANDARD 11)

# Esta es la ruta CORRECTA a tu juego
add_library(main SHARED 
    src/main/cpp/native-lib.cpp
)

target_link_libraries(main 
    android 
    log 
    EGL 
    GLESv2 
    OpenSLES
)
