#pragma once
#include "../third_party/Lab_2/library/include/DynamicArray.hpp"
#include <iostream>
#include <string>

template <typename T>
class Matrix {
private:
    size_t n;                           
    DynamicArray<DynamicArray<T>> data; 
    
public:
    Matrix(size_t size = 2);
    Matrix(std::initializer_list<std::initializer_list<T>> init);
    Matrix(const Matrix& other);
    ~Matrix() = default;
    
    Matrix<T>& operator=(const Matrix<T>& other);
    
    T& operator()(size_t i, size_t j);
    const T& operator()(size_t i, size_t j) const;
    
    size_t GetSize() const;
    
    Matrix<T> operator+(const Matrix<T>& other) const;
    Matrix<T> operator*(const Matrix<T>& other) const;
    Matrix<T> operator*(const T& scalar) const;
    
    bool operator==(const T& zero) const;
    bool operator==(const Matrix<T>& other) const;
    
    static Matrix<T> Identity(size_t size);
    
    bool IsIdentity() const;
    
    friend std::ostream& operator<<(std::ostream& os, const Matrix<T>& m) {
        os << "[";
        for (size_t i = 0; i < m.n; ++i) {
            if (i > 0) os << "; ";
            os << "[";
            for (size_t j = 0; j < m.n; ++j) {
                if (j > 0) os << ", ";
                os << m(i, j);
            }
            os << "]";
        }
        os << "]";
        return os;
    }
};

#include "Matrix.tpp"