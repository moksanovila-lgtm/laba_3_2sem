#include "Matrix.hpp"

template <typename T>
Matrix<T>::Matrix(size_t size) : n(size) {
    if (size == 0) return;
    data.Resize(n);
    for (size_t i = 0; i < n; ++i) {
        DynamicArray<T> row;
        row.Resize(n);
        for (size_t j = 0; j < n; ++j) {
            row.Set(j, T(0));
        }
        data.Set(i, row);
    }
}

template <typename T>
Matrix<T>::Matrix(std::initializer_list<std::initializer_list<T>> init) {
    n = init.size();
    data.Resize(n);
    size_t i = 0;
    for (const auto& rowInit : init) {
        DynamicArray<T> row;
        row.Resize(n);
        size_t j = 0;
        for (const T& val : rowInit) {
            if (j < n) row.Set(j, val);
            ++j;
        }
        data.Set(i, row);
        ++i;
    }
}

template <typename T>
Matrix<T>::Matrix(const Matrix& other) : n(other.n) {
    data.Resize(n);
    for (size_t i = 0; i < n; ++i) {
        DynamicArray<T> row;
        row.Resize(n);
        for (size_t j = 0; j < n; ++j) {
            row.Set(j, other(i, j));
        }
        data.Set(i, row);
    }
}

template <typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix& other) {
    if (this != &other) {
        n = other.n;
        data.Resize(n);
        for (size_t i = 0; i < n; ++i) {
            DynamicArray<T> row;
            row.Resize(n);
            for (size_t j = 0; j < n; ++j) {
                row.Set(j, other(i, j));
            }
            data.Set(i, row);
        }
    }
    return *this;
}

template <typename T>
T& Matrix<T>::operator()(size_t i, size_t j) {
    if (i >= n) {
        throw std::out_of_range(
            "Matrix index out of range: row " + std::to_string(i) + 
            " >= size " + std::to_string(n)
        );
    }
    if (j >= n) {
        throw std::out_of_range(
            "Matrix index out of range: column " + std::to_string(j) + 
            " >= size " + std::to_string(n)
        );
    }
    return data.Get(i).Get(j);
}

template <typename T>
const T& Matrix<T>::operator()(size_t i, size_t j) const {
    if (i >= n) {
        throw std::out_of_range(
            "Matrix index out of range: row " + std::to_string(i) + 
            " >= size " + std::to_string(n)
        );
    }
    if (j >= n) {
        throw std::out_of_range(
            "Matrix index out of range: column " + std::to_string(j) + 
            " >= size " + std::to_string(n)
        );
    }
    return data.Get(i).Get(j);
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
        throw std::invalid_argument(
            "Matrix sizes different: " + std::to_string(n) + 
            " vs " + std::to_string(other.n)
        );
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
        throw std::invalid_argument(
            "Matrix sizes different: " + std::to_string(n) + 
            " vs " + std::to_string(other.n)
        );
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
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            result(i, j) = (*this)(i, j) * scalar;
        }
    }
    return result;
}

template <typename T>
bool Matrix<T>::operator==(const T& zero) const {
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if ((*this)(i, j) != zero) return false;
        }
    }
    return true;
}

template <typename T>
bool Matrix<T>::operator==(const Matrix& other) const {
    if (n != other.n) return false;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if ((*this)(i, j) != other(i, j)) return false;
        }
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