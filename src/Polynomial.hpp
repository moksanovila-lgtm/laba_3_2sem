#pragma once
#include "../third_party/Lab_2/library/include/ArraySequence.hpp"
#include <string>
#include <sstream>

template <typename T>
class Polynomial : public ArraySequence<T> {
private:
    int degree;
    
    void UpdateDegree();
    
public:
    Polynomial();
    Polynomial(std::initializer_list<T> coeffs);
    
    int GetDegree() const;
    T GetCoefficient(size_t index) const;
    
    Polynomial<T> Add(const Polynomial<T>& other) const;
    Polynomial<T> Multiply(const Polynomial<T>& other) const;
    Polynomial<T> MultiplyByScalar(const T& scalar) const;
    
    T Evaluate(const T& x) const;
    
    Polynomial<T> Compose(const Polynomial<T>& other) const;
    
    Polynomial<T> operator+(const Polynomial<T>& other) const;
    Polynomial<T> operator*(const Polynomial<T>& other) const;
    Polynomial<T> operator*(const T& scalar) const;
    
    std::string ToString() const;
};

#include "Polynomial.tpp"