#pragma once

#include "../third_party/Lab_2/library/include/ArraySequence.hpp"
#include "../third_party/Lab_2/library/include/Optional.hpp"
#include <string>
#include <sstream>
#include <concepts>
#include <initializer_list>
#include <functional>

template<typename Seq, typename T>
concept PolynomialContainer = requires(Seq s, T value, size_t idx) {
    { s.GetCount() } -> std::convertible_to<size_t>;    
    { s.Get(idx) } -> std::convertible_to<T>;           
    { s.Append(value) } -> std::same_as<Seq*>;          
};

template <template<typename> class Container, typename T>
requires PolynomialContainer<Container<T>, T>
class Polynomial {
private:
    Container<T> coefficients;
    Optional<int> degree;
    void UpdateDegree();
    
public:
    Polynomial();
    Polynomial(std::initializer_list<T> coeffs);
    Polynomial(const Polynomial& other);
    
    int GetDegree() const;
    T GetCoefficient(size_t index) const;
    size_t GetCount() const;
    
    Polynomial<Container, T> Add(const Polynomial<Container, T>& other) const;
    Polynomial<Container, T> Multiply(const Polynomial<Container, T>& other) const;
    Polynomial<Container, T> MultiplyByScalar(const T& scalar) const;
    
    T Evaluate(const T& x) const;
    
    Polynomial<Container, T> Compose(const Polynomial<Container, T>& other) const;
    
    Polynomial<Container, T> operator+(const Polynomial<Container, T>& other) const;
    Polynomial<Container, T> operator*(const Polynomial<Container, T>& other) const;
    Polynomial<Container, T> operator*(const T& scalar) const;
    Polynomial& operator=(const Polynomial& other);
    
    std::string ToString() const;

    Polynomial<Container, T>* Append(const T& item) {
    coefficients.Append(item);
    UpdateDegree();
    return this;
    }

    T Get(size_t index) const {
        return coefficients.Get(index);
    }

};

#include "Polynomial.tpp"