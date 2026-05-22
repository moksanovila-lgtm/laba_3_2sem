#include <string>
#include <sstream>
#include <iomanip>      
#include <cmath>  
#include <algorithm>
#include <complex>
#include "Polynomial.hpp"
#include "Matrix.hpp"

template <typename T>
Polynomial<T>::Polynomial() 
    : ArraySequence<T>(), degree(-1) {}

template <typename T>
Polynomial<T>::Polynomial(std::initializer_list<T> coeffs) 
    : ArraySequence<T>(coeffs), degree(-1) {
    UpdateDegree();
}

template <typename T>
void Polynomial<T>::UpdateDegree() {
    degree = -1;
    for (int i = this->GetCount() - 1; i >= 0; --i) {
        if (this->Get(i) != T(0)) {
            degree = i;
            return;
        }
    }
}

template <typename T>
int Polynomial<T>::GetDegree() const {
    return degree < 0 ? 0 : degree;
}

template <typename T>
T Polynomial<T>::GetCoefficient(size_t index) const {
    if (index >= this->GetCount()) return T(0);
    return this->Get(index);
}

template <typename T>
Polynomial<T> Polynomial<T>::Add(const Polynomial<T>& other) const {
    size_t maxSize = std::max(this->GetCount(), other.GetCount());
    Polynomial<T> result;
    
    for (size_t i = 0; i < maxSize; ++i) {
        T sum = GetCoefficient(i) + other.GetCoefficient(i);
        result.Append(sum);
    }
    
    return result;
}

template <typename T>
Polynomial<T> Polynomial<T>::Multiply(const Polynomial<T>& other) const {
    if (this->GetCount() == 0 || other.GetCount() == 0) {
        return Polynomial<T>();
    }
    
    size_t newSize = this->GetCount() + other.GetCount() - 1;
    Polynomial<T> result;
    
    for (size_t i = 0; i < newSize; ++i) {
        result.Append(T(0));
    }

    for (size_t i = 0; i < this->GetCount(); ++i) {
        for (size_t j = 0; j < other.GetCount(); ++j) {
            T val = result.Get(i + j) + (this->Get(i) * other.Get(j));
            result.Set(i + j, val);
        }
    }
    
    return result;
}

template <typename T>
Polynomial<T> Polynomial<T>::MultiplyByScalar(const T& scalar) const {
    Polynomial<T> result;
    for (size_t i = 0; i < this->GetCount(); ++i) {
        result.Append(this->Get(i) * scalar);
    }
    return result;
}

template <typename T>
T Polynomial<T>::Evaluate(const T& x) const {
    if (this->GetCount() == 0) return T(0);
    
    T result = this->Get(this->GetCount() - 1);
    for (int i = this->GetCount() - 2; i >= 0; --i) {
        result = result * x + this->Get(i);
    }
    return result;
}


template <>
inline Matrix<double> Polynomial<Matrix<double>>::Evaluate(const Matrix<double>& x) const {
    if (this->GetCount() == 0) return Matrix<double>();
    Matrix<double> result = this->Get(this->GetCount() - 1);
    for (int i = this->GetCount() - 2; i >= 0; --i) {
        result = result * x + this->Get(i);
    }
    return result;
}

template <typename T>
Polynomial<T> Polynomial<T>::Compose(const Polynomial<T>& other) const {
    Polynomial<T> result;
    
    for (size_t i = 0; i < this->GetCount(); ++i) {
        if (i == 0) {
            result = result.Add(Polynomial<T>{this->Get(0)});
        } else if (i == 1) {
            result = result.Add(other.MultiplyByScalar(this->Get(1)));
        } else {
            Polynomial<T> power = other;
            for (size_t j = 1; j < i; ++j) {
                power = power.Multiply(other);
            }
            result = result.Add(power.MultiplyByScalar(this->Get(i)));
        }
    }
    
    return result;
}

template <typename T>
Polynomial<T> Polynomial<T>::operator+(const Polynomial<T>& other) const {
    return Add(other);
}

template <typename T>
Polynomial<T> Polynomial<T>::operator*(const Polynomial<T>& other) const {
    return Multiply(other);
}

template <typename T>
Polynomial<T> Polynomial<T>::operator*(const T& scalar) const {
    return MultiplyByScalar(scalar);
}

template <typename T>
inline std::string Polynomial<T>::ToString() const {
    if (this->GetCount() == 0) return "0";
    
    std::stringstream ss;
    bool first = true;
    
    for (int i = this->GetCount() - 1; i >= 0; --i) {
        T coeff = this->Get(i);
        if (coeff == T(0)) continue;
        
        if (!first) {
            if (coeff > T(0)) ss << "+";
        }
        first = false;
        
        if (i == 0) {
            ss << coeff;
        } else if (i == 1) {
            if (coeff == T(1)) ss << "x";
            else if (coeff == T(-1)) ss << "-x";
            else ss << coeff << "x";
        } else {
            if (coeff == T(1)) ss << "x^" << i;
            else if (coeff == T(-1)) ss << "-x^" << i;
            else ss << coeff << "x^" << i;
        }
    }
    
    if (first) return "0";
    return ss.str();
}

inline std::string formatDouble(double value) {
    std::stringstream ss;
    if (std::abs(value - std::round(value)) < 1e-10) {
        ss << static_cast<long long>(value);
    } else {
        ss << std::fixed << std::setprecision(2) << value;
    }
    return ss.str();
}

template <>
inline std::string Polynomial<std::complex<double>>::ToString() const {
    if (this->GetCount() == 0) return "0";
    
    std::stringstream ss;
    bool first = true;
    
    for (int i = this->GetCount() - 1; i >= 0; --i) {
        std::complex<double> coeff = this->Get(i);
        if (coeff == std::complex<double>(0, 0)) continue;
        
        if (!first) ss << "+";
        first = false;
        
        std::string realStr = formatDouble(coeff.real());
        std::string imagStr = formatDouble(std::abs(coeff.imag()));
        
        std::string complexStr;
        if (coeff.imag() == 0) {
            complexStr = realStr;
        } else if (coeff.real() == 0) {
            complexStr = imagStr + "i";
            if (coeff.imag() < 0) complexStr = "-" + complexStr;
        } else {
            complexStr = realStr;
            if (coeff.imag() > 0) complexStr += "+";
            else complexStr += "-";
            complexStr += imagStr + "i";
        }
        
        if (i == 0) {
            ss << "(" << complexStr << ")";
        } else if (i == 1) {
            if (complexStr == "1") ss << "x";
            else if (complexStr == "-1") ss << "-x";
            else ss << "(" << complexStr << ")x";
        } else {
            if (complexStr == "1") ss << "x^" << i;
            else if (complexStr == "-1") ss << "-x^" << i;
            else ss << "(" << complexStr << ")x^" << i;
        }
    }
    
    if (first) return "0";
    return ss.str();
}

template <>
inline std::string Polynomial<Matrix<double>>::ToString() const {
    if (this->GetCount() == 0) return "0";
    
    std::stringstream ss;
    bool first = true;
    
    for (int i = this->GetCount() - 1; i >= 0; --i) {
        Matrix<double> coeff = this->Get(i);
        
        bool isZero = true;
        for (size_t r = 0; r < coeff.GetSize() && isZero; ++r) {
            for (size_t c = 0; c < coeff.GetSize() && isZero; ++c) {
                if (coeff(r, c) != 0) isZero = false;
            }
        }
        
        if (isZero) continue;
        
        if (!first) ss << "+";
        first = false;
        
        if (i == 0) {
            ss << coeff;
        } else if (i == 1) {
            ss << coeff << "x";
        } else {
            ss << coeff << "x^" << i;
        }
    }
    
    if (first) return "0";
    return ss.str();
}

template <typename T>
Polynomial<T>& Polynomial<T>::operator=(const Polynomial<T>& other) {
    if (this != &other) {
        this->Clear();
     
        for (size_t i = 0; i < other.GetCount(); ++i) {
            this->Append(other.Get(i));
        }
        
        degree = other.degree;
    }
    return *this;
}

