cmake_minimum_required(VERSION 3.16)
project(PolynomialGUI)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/PolynomialController.cpp
    src/PolynomialWidget.cpp
    src/OperationDialog.cpp
    src/MatrixInputDialog.cpp
)

set(HEADERS
    include/MainWindow.hpp
    include/PolynomialController.hpp
    include/PolynomialWidget.hpp
    include/OperationDialog.hpp
    include/MatrixInputDialog.hpp
)

add_executable(polynomial_gui ${SOURCES} ${HEADERS})

target_include_directories(polynomial_gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include   
    ${CMAKE_CURRENT_SOURCE_DIR}/../src          
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/Lab_2/library/include 
)

target_link_libraries(polynomial_gui
    Qt6::Widgets
)