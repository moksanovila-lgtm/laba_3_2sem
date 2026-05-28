#include "Matrix.hpp"
#include "exceptions.hpp"
#include <stdexcept>

template <typename T>
Matrix<T>::Matrix(size_t size) : n(size), data(n * n) {}

template <typename T>
Matrix<T>::Matrix(std::initializer_list<std::initializer_list<T>> init) : n(init.size()), data(n * n) {
    size_t i = 0;
    for (const auto& row : init) {
        size_t j = 0;
        for (const T& val : row) {
            if (i < n && j < n) {
                (*this)(i, j) = val;
            }
            ++j;
        }
        ++i;
    }
}

template <typename T>
Matrix<T>::Matrix(const Matrix& other) : n(other.n), data(other.data) {}

template <typename T>
T& Matrix<T>::operator()(size_t i, size_t j) {
    if (i >= n || j >= n) {
        throw MatrixIndexOutOfRangeException(i, j, n);
    }
    return data.Get(i * n + j);
}

template <typename T>
const T& Matrix<T>::operator()(size_t i, size_t j) const {
    if (i >= n || j >= n) {
        throw MatrixIndexOutOfRangeException(i, j, n);
    }
    size_t index = i * n + j;
    const T& result = data.Get(index);
    return result;
}

template <typename T>
size_t Matrix<T>::GetSize() const {
    return n;
}

template <typename T>
Matrix<T> Matrix<T>::operator+(const Matrix& other) const {
    if (n == 0) return other;
    if (other.n == 0) return *this;
    if (n != other.n) {
        throw MatrixSizeMismatchException(n, other.n);
    }
    Matrix result(n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            result(i, j) = (*this)(i, j) + other(i, j);
        }
    }
    return result;
}

template <typename T>
Matrix<T> Matrix<T>::operator*(const Matrix& other) const {
    if (n == 0 || other.n == 0) return Matrix<T>();
    if (n != other.n) {
        throw MatrixSizeMismatchException(n, other.n);
    }
    Matrix result(n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            T sum = T(0);
            for (size_t k = 0; k < n; ++k) {
                sum = sum + (*this)(i, k) * other(k, j);
            }
            result(i, j) = sum;
        }
    }
    return result;
}

template <typename T>
Matrix<T> Matrix<T>::operator*(const T& scalar) const {
    Matrix result(n);
    for (size_t i = 0; i < n * n; ++i) {
        result.data.Set(i, data.Get(i) * scalar);
    }
    return result;
}

template <typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix& other) {
    if (this != &other) {
        n = other.n;
        data = other.data;
    }
    return *this;
}

template <typename T>
bool Matrix<T>::operator==(const T& zero) const {
    for (size_t i = 0; i < n * n; ++i) {
        if (data.Get(i) != zero) return false;
    }
    return true;
}

template <typename T>
bool Matrix<T>::operator==(const Matrix& other) const {
    if (n != other.n) return false;
    for (size_t i = 0; i < n * n; ++i) {
        if (data.Get(i) != other.data.Get(i)) return false;
    }
    return true;
}

template <typename T>
bool Matrix<T>::operator!=(const Matrix& other) const {
    return !(*this == other);
}

template <typename T>
Matrix<T> Matrix<T>::Identity(size_t size) {
    Matrix result(size);
    for (size_t i = 0; i < size; ++i) {
        result(i, i) = T(1);
    }
    return result;
}

template <typename T>
bool Matrix<T>::IsIdentity() const {
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (i == j) {
                if ((*this)(i, j) != T(1)) return false;
            } else {
                if ((*this)(i, j) != T(0)) return false;
            }
        }
    }
    return true;
}

template <typename T>
Matrix<T>::operator std::string() const {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < n; ++i) {
        if (i > 0) ss << "; ";
        ss << "(";
        for (size_t j = 0; j < n; ++j) {
            if (j > 0) ss << ", ";
            ss << (*this)(i, j);
        }
        ss << ")";
    }
    ss << "]";
    std::string result = ss.str();
    return result;
}