cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(FS)
add_compile_options(-std=c++11)
set(ecat /opt/icpdas/ecat_m801)

include_directories(${ecat}/include)
link_directories(${ecat}/lib)

add_executable(project FSenser.cpp)



# target_link_libraries(project liblibecatdevice.a)
target_link_libraries(project ecatdevice pthread)