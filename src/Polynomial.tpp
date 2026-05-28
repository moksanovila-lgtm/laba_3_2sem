#include "Polynomial.hpp"
#include "Matrix.hpp"
#include <string>
#include <sstream>
#include <algorithm>
#include <complex>
#include <iomanip>
#include <cmath>

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
Polynomial<Container, T>::Polynomial() 
    : coefficients(), degree(std::nullopt) {}

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
Polynomial<Container, T>::Polynomial(std::initializer_list<T> coeffs) 
    : coefficients() {
    for (const T& c : coeffs) {
        coefficients.Append(c);
    }
    UpdateDegree();
}

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
Polynomial<Container, T>::Polynomial(const Polynomial& other)
    : coefficients() {
    for (size_t i = 0; i < other.coefficients.GetCount(); ++i) {
        coefficients.Append(other.coefficients.Get(i));
    }
    degree = other.degree;
}

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
void Polynomial<Container, T>::UpdateDegree() {
    degree.reset();  
    for (int i = coefficients.GetCount() - 1; i >= 0; --i) {
        if (coefficients.Get(i) != T(0)) {
            degree = i;
            return;
        }
    }
}

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
int Polynomial<Container, T>::GetDegree() const {
    return degree.value_or(0);  
}

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
T Polynomial<Container, T>::GetCoefficient(size_t index) const {
    if (index >= coefficients.GetCount()) return T(0); 
    return coefficients.Get(index);
}

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
size_t Polynomial<Container, T>::GetCount() const {
    return coefficients.GetCount();
}

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
Polynomial<Container, T> Polynomial<Container, T>::Add(const Polynomial<Container, T>& other) const {
    size_t maxSize = std::max(GetCount(), other.GetCount());
    Polynomial<Container, T> result;
    for (size_t i = 0; i < maxSize; ++i) {
        T sum = GetCoefficient(i) + other.GetCoefficient(i);
        result.coefficients.Append(sum);
    }
    return result;
}

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
Polynomial<Container, T> Polynomial<Container, T>::Multiply(const Polynomial<Container, T>& other) const {
    if (GetCount() == 0 || other.GetCount() == 0) {
        return Polynomial<Container, T>();
    }
    size_t newSize = GetCount() + other.GetCount() - 1;
    DynamicArray<T> temp(newSize);
    for (size_t i = 0; i < newSize; ++i) {
        temp.Set(i, T(0));
    }
    for (size_t i = 0; i < GetCount(); ++i) {
        for (size_t j = 0; j < other.GetCount(); ++j) {
            T val = temp.Get(i + j) + (GetCoefficient(i) * other.GetCoefficient(j));
            temp.Set(i + j, val);
        }
    }
    Polynomial<Container, T> result;
    for (size_t i = 0; i < newSize; ++i) {
        result.coefficients.Append(temp.Get(i));
    }
    return result;
}

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
Polynomial<Container, T> Polynomial<Container, T>::MultiplyByScalar(const T& scalar) const {
    Polynomial<Container, T> result;
    for (size_t i = 0; i < GetCount(); ++i) {
        result.coefficients.Append(GetCoefficient(i) * scalar);
    }
    return result;
}

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
T Polynomial<Container, T>::Evaluate(const T& x) const {
    if (GetCount() == 0) return T(0);
    T result = GetCoefficient(GetCount() - 1);
    for (int i = GetCount() - 2; i >= 0; --i) {
        result = result * x + GetCoefficient(i);
    }
    return result;
}

template <>
inline Matrix<double> Polynomial<ArraySequence, Matrix<double>>::Evaluate(const Matrix<double>& x) const {
    if (GetCount() == 0) return Matrix<double>();
    Matrix<double> result = GetCoefficient(GetCount() - 1);
    for (int i = GetCount() - 2; i >= 0; --i) {
        result = result * x + GetCoefficient(i);
    }
    return result;
}

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
Polynomial<Container, T> Polynomial<Container, T>::Compose(const Polynomial<Container, T>& other) const {
    Polynomial<Container, T> result;
    for (size_t i = 0; i < GetCount(); ++i) {
        if (i == 0) {
            result = result.Add(Polynomial<Container, T>{GetCoefficient(0)});
        } else if (i == 1) {
            result = result.Add(other.MultiplyByScalar(GetCoefficient(1)));
        } else {
            Polynomial<Container, T> power = other;
            for (size_t j = 1; j < i; ++j) {
                power = power.Multiply(other);
            }
            result = result.Add(power.MultiplyByScalar(GetCoefficient(i)));
        }
    }
    return result;
}

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
Polynomial<Container, T> Polynomial<Container, T>::operator+(const Polynomial<Container, T>& other) const {
    return Add(other);
}

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
Polynomial<Container, T> Polynomial<Container, T>::operator*(const Polynomial<Container, T>& other) const {
    return Multiply(other);
}

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
Polynomial<Container, T> Polynomial<Container, T>::operator*(const T& scalar) const {
    return MultiplyByScalar(scalar);
}

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
Polynomial<Container, T>& Polynomial<Container, T>::operator=(const Polynomial& other) {
    if (this != &other) {
        coefficients = Container<T>(other.coefficients);
        degree = other.degree;
    }
    return *this;
}

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
std::string Polynomial<Container, T>::ToString() const {
    if (GetCount() == 0) return "0";
    std::stringstream ss;
    bool first = true;
    for (int i = GetCount() - 1; i >= 0; --i) {
        T coeff = GetCoefficient(i);
        if (coeff == T(0)) continue;
        if (!first) {
            if constexpr (std::is_arithmetic_v<T>) {
                if (coeff > T(0)) ss << "+";
            } else {
                ss << "+";
            }
        }
        first = false;
        if (i == 0) {
            ss << coeff;
        } else if (i == 1) {
            if constexpr (std::is_arithmetic_v<T>) {
                if (coeff == T(1)) ss << "x";
                else if (coeff == T(-1)) ss << "-x";
                else ss << coeff << "x";
            } else {
                ss << coeff << "x";
            }
        } else {
            if constexpr (std::is_arithmetic_v<T>) {
                if (coeff == T(1)) ss << "x^" << i;
                else if (coeff == T(-1)) ss << "-x^" << i;
                else ss << coeff << "x^" << i;
            } else {
                ss << coeff << "x^" << i;
            }
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
inline std::string Polynomial<ArraySequence, std::complex<double>>::ToString() const {
    if (GetCount() == 0) return "0";
    std::stringstream ss;
    bool first = true;
    for (int i = GetCount() - 1; i >= 0; --i) {
        std::complex<double> coeff = GetCoefficient(i);
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
inline std::string Polynomial<ArraySequence, Matrix<double>>::ToString() const {
    if (GetCount() == 0) {
        return "0";
    }
    std::stringstream ss;
    bool first = true;
    for (int i = GetCount() - 1; i >= 0; --i) {
        Matrix<double> coeff = GetCoefficient(i);
        bool isZero = true;
        for (size_t r = 0; r < coeff.GetSize() && isZero; ++r) {
            for (size_t c = 0; c < coeff.GetSize() && isZero; ++c) {
                if (coeff(r, c) != 0.0) isZero = false;
            }
        }
        if (isZero) {
            continue;
        }
        
        if (!first) {
            ss << "+";
        }
        first = false;
        if (i == 0) {
            ss << coeff;
        } else if (i == 1) {
            ss << coeff << "x";
        } else {
            ss << coeff << "x^" << i;
        }
    }
    if (first) {
        return "0";
    }
    std::string result = ss.str();
    return result;
}