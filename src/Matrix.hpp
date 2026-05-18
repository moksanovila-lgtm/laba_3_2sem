#pragma once

#include "DynamicArray.hpp"
#include <iostream>
#include <string>
#include <stdexcept>

template <typename T>
class Matrix {
private:
    size_t n;
    DynamicArray<DynamicArray<T>> data;
    
public:
    Matrix(size_t size = 2);
    Matrix(std::initializer_list<std::initializer_list<T>> init);
    Matrix(const Matrix& other);
    Matrix& operator=(const Matrix& other);
    
    T& operator()(size_t i, size_t j);
    const T& operator()(size_t i, size_t j) const;
    size_t GetSize() const;
    
    Matrix operator+(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    Matrix operator*(const T& scalar) const;
    
    bool operator==(const T& zero) const;
    bool operator==(const Matrix& other) const;
    bool operator!=(const Matrix& other) const;
    
    static Matrix Identity(size_t size);
    bool IsIdentity() const;
    
    friend std::ostream& operator<<(std::ostream& os, const Matrix& m) {
        os << "[";
        for (size_t i = 0; i < m.GetSize(); ++i) {
            if (i > 0) os << "; ";
            os << "(";
            for (size_t j = 0; j < m.GetSize(); ++j) {
                if (j > 0) os << ", ";
                os << m(i, j);
            }
            os << ")";
        }
        os << "]";
        return os;
    }

};

#include "Matrix.tpp"