#include "Matrix.hpp"

template <typename T>
Matrix<T>::Matrix(size_t size) : n(size) {
    for (size_t i = 0; i < n; ++i) {
        DynamicArray<T> row;
        for (size_t j = 0; j < n; ++j) {
            row.Append(T(0));
        }
        data.Append(row);
    }
}

template <typename T>
Matrix<T>::Matrix(std::initializer_list<std::initializer_list<T>> init) {
    n = init.size();
    for (const auto& row : init) {
        DynamicArray<T> newRow;
        for (const T& val : row) {
            newRow.Append(val);
        }
        data.Append(newRow);
    }
}

template <typename T>
Matrix<T>::Matrix(const Matrix& other) : n(other.n) {
    for (size_t i = 0; i < n; ++i) {
        DynamicArray<T> row;
        for (size_t j = 0; j < n; ++j) {
            row.Append(other(i, j));
        }
        data.Append(row);
    }
}

template <typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& other) {
    if (this != &other) {
        data.Clear();
        n = other.n;
        for (size_t i = 0; i < n; ++i) {
            DynamicArray<T> row;
            for (size_t j = 0; j < n; ++j) {
                row.Append(other(i, j));
            }
            data.Append(row);
        }
    }
    return *this;
}

template <typename T>
T& Matrix<T>::operator()(size_t i, size_t j) {
    return data.Get(i).Get(j);
}

template <typename T>
const T& Matrix<T>::operator()(size_t i, size_t j) const {
    return data.Get(i).Get(j);
}

template <typename T>
size_t Matrix<T>::GetSize() const {
    return n;
}

template <typename T>
Matrix<T> Matrix<T>::operator+(const Matrix<T>& other) const {
    if (n != other.n) {
        throw std::invalid_argument("Matrix sizes differ");
    }
    Matrix<T> result(n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            result(i, j) = (*this)(i, j) + other(i, j);
        }
    }
    return result;
}

template <typename T>
Matrix<T> Matrix<T>::operator*(const Matrix<T>& other) const {
    if (n != other.n) {
        throw std::invalid_argument("Matrix sizes differ");
    }
    Matrix<T> result(n);
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
    Matrix<T> result(n);
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
bool Matrix<T>::operator==(const Matrix<T>& other) const {
    if (n != other.n) return false;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if ((*this)(i, j) != other(i, j)) return false;
        }
    }
    return true;
}

template <typename T>
Matrix<T> Matrix<T>::Identity(size_t size) {
    Matrix<T> result(size);
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