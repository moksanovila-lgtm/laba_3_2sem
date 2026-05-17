cmake_minimum_required(VERSION 3.16)
project(Lab3_Polynomial)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

set(LAB_2_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/Lab_2/library/include)
set(MODEL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(app_gui)
add_subdirectory(tests)

add_library(PolynomialModel INTERFACE)
target_include_directories(PolynomialModel INTERFACE
    ${LAB_2_INCLUDE}
    ${MODEL_SRC}
)