// #include "Matrix.hpp"

// template <typename T>
// Matrix<T>::Matrix(size_t size) : n(size) {
//     data.Resize(n);
//     for (size_t i = 0; i < n; ++i) {
//         DynamicArray<T> row;
//         row.Resize(n);
//         for (size_t j = 0; j < n; ++j) {
//             row.Set(j, T(0));
//         }
//         data.Set(i, row);
//     }
// }

// template <typename T>
// Matrix<T>::Matrix(std::initializer_list<std::initializer_list<T>> init) {
//     n = init.size();
//     data.Resize(n);
//     size_t i = 0;
//     for (const auto& rowInit : init) {
//         DynamicArray<T> row;
//         row.Resize(n);
//         size_t j = 0;
//         for (const T& val : rowInit) {
//             if (j < n) row.Set(j, val);
//             ++j;
//         }
//         data.Set(i, row);
//         ++i;
//     }
// }

// template <typename T>
// Matrix<T>::Matrix(const Matrix& other) : n(other.n) {
//     data.Resize(n);
//     for (size_t i = 0; i < n; ++i) {
//         DynamicArray<T> row;
//         row.Resize(n);
//         for (size_t j = 0; j < n; ++j) {
//             row.Set(j, other(i, j));
//         }
//         data.Set(i, row);
//     }
// }

// template <typename T>
// Matrix<T>& Matrix<T>::operator=(const Matrix& other) {
//     if (this != &other) {
//         n = other.n;
//         data.Resize(n);
//         for (size_t i = 0; i < n; ++i) {
//             DynamicArray<T> row;
//             row.Resize(n);
//             for (size_t j = 0; j < n; ++j) {
//                 row.Set(j, other(i, j));
//             }
//             data.Set(i, row);
//         }
//     }
//     return *this;
// }

// template <typename T>
// T& Matrix<T>::operator()(size_t i, size_t j) {
//     return data.Get(i).Get(j);
// }

// template <typename T>
// const T& Matrix<T>::operator()(size_t i, size_t j) const {
//     return data.Get(i).Get(j);
// }

// template <typename T>
// size_t Matrix<T>::GetSize() const {
//     return n;
// }

// template <typename T>
// Matrix<T> Matrix<T>::operator+(const Matrix& other) const {
//     if (n != other.n) {
//         throw std::invalid_argument("Matrix sizes differ");
//     }
//     Matrix result(n);
//     for (size_t i = 0; i < n; ++i) {
//         for (size_t j = 0; j < n; ++j) {
//             result(i, j) = (*this)(i, j) + other(i, j);
//         }
//     }
//     return result;
// }

// template <typename T>
// Matrix<T> Matrix<T>::operator*(const Matrix& other) const {
//     if (n != other.n) {
//         throw std::invalid_argument("Matrix sizes differ");
//     }
//     Matrix result(n);
//     for (size_t i = 0; i < n; ++i) {
//         for (size_t j = 0; j < n; ++j) {
//             T sum = T(0);
//             for (size_t k = 0; k < n; ++k) {
//                 sum = sum + (*this)(i, k) * other(k, j);
//             }
//             result(i, j) = sum;
//         }
//     }
//     return result;
// }

// template <typename T>
// Matrix<T> Matrix<T>::operator*(const T& scalar) const {
//     Matrix result(n);
//     for (size_t i = 0; i < n; ++i) {
//         for (size_t j = 0; j < n; ++j) {
//             result(i, j) = (*this)(i, j) * scalar;
//         }
//     }
//     return result;
// }

// template <typename T>
// bool Matrix<T>::operator==(const T& zero) const {
//     for (size_t i = 0; i < n; ++i) {
//         for (size_t j = 0; j < n; ++j) {
//             if ((*this)(i, j) != zero) return false;
//         }
//     }
//     return true;
// }

// template <typename T>
// bool Matrix<T>::operator==(const Matrix& other) const {
//     if (n != other.n) return false;
//     for (size_t i = 0; i < n; ++i) {
//         for (size_t j = 0; j < n; ++j) {
//             if ((*this)(i, j) != other(i, j)) return false;
//         }
//     }
//     return true;
// }

// template <typename T>
// Matrix<T> Matrix<T>::Identity(size_t size) {
//     Matrix result(size);
//     for (size_t i = 0; i < size; ++i) {
//         result(i, i) = T(1);
//     }
//     return result;
// }

// template <typename T>
// bool Matrix<T>::operator!=(const Matrix& other) const {
//     return !(*this == other);
// }

// #ifdef _GLIBCXX_COMPLEX

// template <typename T>
// bool Matrix<T>::IsIdentity() const {
//     for (size_t i = 0; i < n; ++i) {
//         for (size_t j = 0; j < n; ++j) {
//             if (i == j) {
//                 if ((*this)(i, j) != T(1)) return false;
//             } else {
//                 if ((*this)(i, j) != T(0)) return false;
//             }
//         }
//     }
//     return true;
// }

// #endif

// #include <string>
// #include <sstream>
// #include <algorithm>
// #include "Polynomial.hpp"
// #include "Matrix.hpp"

// template <typename T>
// Polynomial<T>::Polynomial() 
//     : ArraySequence<T>(), degree(-1) {}

// template <typename T>
// Polynomial<T>::Polynomial(std::initializer_list<T> coeffs) 
//     : ArraySequence<T>(coeffs), degree(-1) {
//     UpdateDegree();
// }

// template <typename T>
// void Polynomial<T>::UpdateDegree() {
//     degree = -1;
//     for (int i = this->GetCount() - 1; i >= 0; --i) {
//         if (this->Get(i) != T(0)) {
//             degree = i;
//             return;
//         }
//     }
// }

// template <typename T>
// int Polynomial<T>::GetDegree() const {
//     return degree < 0 ? 0 : degree;
// }

// template <typename T>
// T Polynomial<T>::GetCoefficient(size_t index) const {
//     if (index >= this->GetCount()) return T(0);
//     return this->Get(index);
// }

// template <typename T>
// Polynomial<T> Polynomial<T>::Add(const Polynomial<T>& other) const {
//     size_t maxSize = std::max(this->GetCount(), other.GetCount());
//     Polynomial<T> result;
    
//     for (size_t i = 0; i < maxSize; ++i) {
//         T sum = GetCoefficient(i) + other.GetCoefficient(i);
//         result.Append(sum);
//     }
    
//     return result;
// }

// template <typename T>
// Polynomial<T> Polynomial<T>::Multiply(const Polynomial<T>& other) const {
//     if (this->GetCount() == 0 || other.GetCount() == 0) {
//         return Polynomial<T>();
//     }
    
//     size_t newSize = this->GetCount() + other.GetCount() - 1;
//     Polynomial<T> result;
    
//     for (size_t i = 0; i < newSize; ++i) {
//         result.Append(T(0));
//     }

//     for (size_t i = 0; i < this->GetCount(); ++i) {
//         for (size_t j = 0; j < other.GetCount(); ++j) {
//             T val = result.Get(i + j) + (this->Get(i) * other.Get(j));
//             result.Set(i + j, val);
//         }
//     }
    
//     return result;
// }

// template <typename T>
// Polynomial<T> Polynomial<T>::MultiplyByScalar(const T& scalar) const {
//     Polynomial<T> result;
//     for (size_t i = 0; i < this->GetCount(); ++i) {
//         result.Append(this->Get(i) * scalar);
//     }
//     return result;
// }

// template <typename T>
// T Polynomial<T>::Evaluate(const T& x) const {
//     if (this->GetCount() == 0) return T(0);
    
//     T result = this->Get(this->GetCount() - 1);
//     for (int i = this->GetCount() - 2; i >= 0; --i) {
//         result = result * x + this->Get(i);
//     }
//     return result;
// }

// template <typename T>
// Polynomial<T> Polynomial<T>::Compose(const Polynomial<T>& other) const {
//     Polynomial<T> result;
    
//     for (size_t i = 0; i < this->GetCount(); ++i) {
//         if (i == 0) {
//             result = result.Add(Polynomial<T>{this->Get(0)});
//         } else if (i == 1) {
//             result = result.Add(other.MultiplyByScalar(this->Get(1)));
//         } else {
//             Polynomial<T> power = other;
//             for (size_t j = 1; j < i; ++j) {
//                 power = power.Multiply(other);
//             }
//             result = result.Add(power.MultiplyByScalar(this->Get(i)));
//         }
//     }
    
//     return result;
// }

// template <typename T>
// Polynomial<T> Polynomial<T>::operator+(const Polynomial<T>& other) const {
//     return Add(other);
// }

// template <typename T>
// Polynomial<T> Polynomial<T>::operator*(const Polynomial<T>& other) const {
//     return Multiply(other);
// }

// template <typename T>
// Polynomial<T> Polynomial<T>::operator*(const T& scalar) const {
//     return MultiplyByScalar(scalar);
// }

// template <typename T>
// std::string Polynomial<T>::ToString() const {
//     if (this->GetCount() == 0) return "0";
    
//     std::stringstream ss;
//     bool first = true;
    
//     for (int i = this->GetCount() - 1; i >= 0; --i) {
//         T coeff = this->Get(i);
//         if (coeff == T(0)) continue;
        
//         if (!first) {
//             if (coeff > T(0)) ss << "+";
//         }
//         first = false;
        
//         if (i == 0) {
//             ss << coeff;
//         } else if (i == 1) {
//             if (coeff == T(1)) ss << "x";
//             else if (coeff == T(-1)) ss << "-x";
//             else ss << coeff << "x";
//         } else {
//             if (coeff == T(1)) ss << "x^" << i;
//             else if (coeff == T(-1)) ss << "-x^" << i;
//             else ss << coeff << "x^" << i;
//         }
//     }
    
//     if (first) return "0";
//     return ss.str();
// }


// #ifdef _GLIBCXX_COMPLEX
// template <>
// std::string Polynomial<std::complex<double>>::ToString() const {
//     if (this->GetCount() == 0) return "0";
    
//     std::stringstream ss;
//     bool first = true;
    
//     for (int i = this->GetCount() - 1; i >= 0; --i) {
//         std::complex<double> coeff = this->Get(i);
//         if (coeff == std::complex<double>(0, 0)) continue;
        
//         if (!first && coeff.imag() >= 0) ss << "+";
//         first = false;
        
//         if (i == 0) {
//             ss << coeff.real();
//             if (coeff.imag() != 0) {
//                 if (coeff.imag() > 0) ss << "+";
//                 ss << coeff.imag() << "i";
//             }
//         } else if (i == 1) {
//             if (coeff.real() != 0) ss << coeff.real();
//             if (coeff.imag() != 0) {
//                 if (coeff.imag() > 0 && coeff.real() != 0) ss << "+";
//                 ss << coeff.imag() << "i";
//             }
//             ss << "x";
//         } else {
//             if (coeff.real() != 0) ss << coeff.real();
//             if (coeff.imag() != 0) {
//                 if (coeff.imag() > 0 && coeff.real() != 0) ss << "+";
//                 ss << coeff.imag() << "i";
//             }
//             ss << "x^" << i;
//         }
//     }
    
//     if (first) return "0";
//     return ss.str();
// }
// #endif


// template <>
// std::string Polynomial<Matrix<int>>::ToString() const {
//     if (this->GetCount() == 0) return "0";
    
//     std::stringstream ss;
//     bool first = true;
    
//     for (int i = this->GetCount() - 1; i >= 0; --i) {
//         Matrix<int> coeff = this->Get(i);
        
//         bool isZero = true;
//         for (size_t r = 0; r < coeff.GetSize() && isZero; ++r) {
//             for (size_t c = 0; c < coeff.GetSize() && isZero; ++c) {
//                 if (coeff(r, c) != 0) isZero = false;
//             }
//         }
        
//         if (isZero) continue;
        
//         if (!first) ss << "+";
//         first = false;
        
//         if (i == 0) {
//             ss << coeff;
//         } else if (i == 1) {
//             ss << coeff << "x";
//         } else {
//             ss << coeff << "x^" << i;
//         }
//     }
    
//     if (first) return "0";
//     return ss.str();
// }

#include "Matrix.hpp"

template <typename T>
Matrix<T>::Matrix(size_t size) : n(size) {
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
Matrix<T> Matrix<T>::operator+(const Matrix& other) const {
    if (n != other.n) {
        throw std::invalid_argument("Matrix sizes differ");
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
    if (n != other.n) {
        throw std::invalid_argument("Matrix sizes differ");
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