cmake_minimum_required(VERSION 3.14)
project(PolynomialTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(LAB_2_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/Lab_2/library/include)

set(LAB_3_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../src)

set(TEST_SOURCES
    test_polynomial.cpp
    test_matrix.cpp
    test_polynomial_matrix.cpp
)

add_executable(polynomial_tests ${TEST_SOURCES})

target_include_directories(polynomial_tests PRIVATE
    ${LAB_2_INCLUDE}
    ${LAB_3_SRC}
)

target_link_libraries(polynomial_tests
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(polynomial_tests)