#pragma once

#include "DynamicArray.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

template <typename T>
class Matrix {
private:
    size_t n;
    DynamicArray<T> data;
    
public:
    Matrix(size_t size = 2);
    Matrix(std::initializer_list<std::initializer_list<T>> init);
    Matrix(const Matrix& other);
    
    T& operator()(size_t i, size_t j);          
    const T& operator()(size_t i, size_t j) const;
    size_t GetSize() const;
    
    Matrix operator+(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    Matrix operator*(const T& scalar) const;
    Matrix& operator=(const Matrix& other);
    
    bool operator==(const T& zero) const;
    bool operator==(const Matrix& other) const;
    bool operator!=(const Matrix& other) const;
    
    static Matrix Identity(size_t size);
    bool IsIdentity() const;
    
    operator std::string() const;

    friend std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    os << std::string(m);
    return os;
}
};

#include "Matrix.tpp"