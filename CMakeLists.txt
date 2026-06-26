cmake_minimum_required(VERSION 3.20)

project(AuraRay VERSION 0.1.0 LANGUAGES CXX)

add_executable(auraray src/main.cpp)

target_compile_features(auraray PRIVATE cxx_std_17)
set_target_properties(auraray PROPERTIES
    CXX_EXTENSIONS OFF
)

if(MSVC)
    target_compile_options(auraray PRIVATE /W4)
else()
    target_compile_options(auraray PRIVATE -Wall -Wextra -pedantic)
endif()
