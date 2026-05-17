#include <gtest/gtest.h>
#include "Polynomial.hpp"
#include "Matrix.hpp"
#include <sstream>
#include <string>

template <typename T>
std::string PolyToStr(Polynomial<T>& p) {
    return p.ToString();
}

class PolynomialTest : public ::testing::Test {
protected:
    void SetUp() override {
        intPoly1 = new Polynomial<int>{1, 2, 3};     
        intPoly2 = new Polynomial<int>{4, 5};         
        intPoly3 = new Polynomial<int>{1, 0, -1};      
        
        doublePoly = new Polynomial<double>{1.5, 2.5, 3.5};
    }
    
    void TearDown() override {
        delete intPoly1;
        delete intPoly2;
        delete intPoly3;
        delete doublePoly;
    }
    
    Polynomial<int>* intPoly1;
    Polynomial<int>* intPoly2;
    Polynomial<int>* intPoly3;
    Polynomial<double>* doublePoly;
};

TEST_F(PolynomialTest, IntConstructor) {
    std::string got = PolyToStr(*intPoly1);
    std::string expected = "3x^2+2x+1";
    EXPECT_EQ(got, expected) << "Constructor: expected " << expected << ", got " << got;
}

TEST_F(PolynomialTest, IntGetCoefficient) {
    EXPECT_EQ(intPoly1->GetCoefficient(0), 1) 
        << "GetCoefficient(0): expected 1, got " << intPoly1->GetCoefficient(0);
    EXPECT_EQ(intPoly1->GetCoefficient(1), 2) 
        << "GetCoefficient(1): expected 2, got " << intPoly1->GetCoefficient(1);
    EXPECT_EQ(intPoly1->GetCoefficient(2), 3) 
        << "GetCoefficient(2): expected 3, got " << intPoly1->GetCoefficient(2);
    EXPECT_EQ(intPoly1->GetCoefficient(5), 0) 
        << "GetCoefficient(5) out of range: expected 0, got " << intPoly1->GetCoefficient(5);
}

TEST_F(PolynomialTest, IntGetDegree) {
    EXPECT_EQ(intPoly1->GetDegree(), 2) 
        << "GetDegree: expected 2, got " << intPoly1->GetDegree();
    EXPECT_EQ(intPoly3->GetDegree(), 2) 
        << "GetDegree with zero coefficient: expected 2, got " << intPoly3->GetDegree();
}

TEST_F(PolynomialTest, IntAdd) {
    std::string before1 = PolyToStr(*intPoly1);
    std::string before2 = PolyToStr(*intPoly2);
    auto result = intPoly1->Add(*intPoly2);
    std::string got = PolyToStr(result);
    std::string expected = "3x^2+7x+5";
    EXPECT_EQ(got, expected) 
        << "Add: " << before1 << " + " << before2 << " = expected " << expected << ", got " << got;
}

TEST_F(PolynomialTest, IntMultiply) {
    std::string before1 = PolyToStr(*intPoly1);
    std::string before2 = PolyToStr(*intPoly2);
    auto result = intPoly1->Multiply(*intPoly2);
    std::string got = PolyToStr(result);
    std::string expected = "15x^3+23x^2+13x+4";
    EXPECT_EQ(got, expected) 
        << "Multiply: " << before1 << " * " << before2 << " = expected " << expected << ", got " << got;
}

TEST_F(PolynomialTest, IntMultiplyByScalar) {
    std::string before = PolyToStr(*intPoly1);
    auto result = intPoly1->MultiplyByScalar(2);
    std::string got = PolyToStr(result);
    std::string expected = "6x^2+4x+2";
    EXPECT_EQ(got, expected) 
        << "MultiplyByScalar(2): input=" << before << ", expected " << expected << ", got " << got;
}

TEST_F(PolynomialTest, IntEvaluate) {
    std::string before = PolyToStr(*intPoly1);
    int result = intPoly1->Evaluate(2);
    EXPECT_EQ(result, 17) 
        << "Evaluate(2): input=" << before << ", expected 17, got " << result;
}

TEST_F(PolynomialTest, IntCompose) {
    std::string before1 = PolyToStr(*intPoly1);
    std::string before2 = PolyToStr(*intPoly2);
    auto result = intPoly1->Compose(*intPoly2);
    std::string got = PolyToStr(result);
    std::string expected = "75x^2+170x+96";
    EXPECT_EQ(got, expected) 
        << "Compose: " << before1 << "(" << before2 << ") = expected " << expected << ", got " << got;
}

TEST_F(PolynomialTest, IntOperatorPlus) {
    std::string before1 = PolyToStr(*intPoly1);
    std::string before2 = PolyToStr(*intPoly2);
    auto result = *intPoly1 + *intPoly2;
    std::string got = PolyToStr(result);
    std::string expected = "3x^2+7x+5";
    EXPECT_EQ(got, expected) 
        << "operator+: " << before1 << " + " << before2 << " = expected " << expected << ", got " << got;
}

TEST_F(PolynomialTest, IntOperatorMultiply) {
    std::string before1 = PolyToStr(*intPoly1);
    std::string before2 = PolyToStr(*intPoly2);
    auto result = *intPoly1 * *intPoly2;
    std::string got = PolyToStr(result);
    std::string expected = "15x^3+23x^2+13x+4";
    EXPECT_EQ(got, expected) 
        << "operator*: " << before1 << " * " << before2 << " = expected " << expected << ", got " << got;
}

TEST_F(PolynomialTest, DoubleEvaluate) {
    std::string before = PolyToStr(*doublePoly);
    double result = doublePoly->Evaluate(2.0);
    double expected = 1.5 + 2.5*2 + 3.5*4;
    EXPECT_DOUBLE_EQ(result, expected) 
        << "Evaluate(2.0): input=" << before << ", expected " << expected << ", got " << result;
}

TEST_F(PolynomialTest, DoubleAdd) {
    Polynomial<double> p1{1.0, 2.0, 3.0};
    Polynomial<double> p2{4.0, 5.0};
    auto result = p1.Add(p2);
    std::string got = PolyToStr(result);
    std::string expected = "3x^2+7x+5";
    EXPECT_EQ(got, expected) 
        << "Double Add: expected " << expected << ", got " << got;
}

TEST(PolynomialBoundaryTest, EmptyPolynomial) {
    Polynomial<int> p;
    std::string got = p.ToString();
    std::string expected = "0";
    EXPECT_EQ(got, expected) << "Empty polynomial: expected " << expected << ", got " << got;
    EXPECT_EQ(p.GetDegree(), 0) << "Empty polynomial degree should be 0";
}

TEST(PolynomialBoundaryTest, ZeroPolynomial) {
    Polynomial<int> p{0, 0, 0};
    std::string got = p.ToString();
    std::string expected = "0";
    EXPECT_EQ(got, expected) << "Zero polynomial: expected " << expected << ", got " << got;
    EXPECT_EQ(p.GetDegree(), 0) << "Zero polynomial degree should be 0";
}

TEST(PolynomialBoundaryTest, PolynomialWithZeroCoefficients) {
    Polynomial<int> p{1, 0, 0, 2};
    std::string got = p.ToString();
    std::string expected = "2x^3+1";
    EXPECT_EQ(got, expected) 
        << "Polynomial with zero coefficients: expected " << expected << ", got " << got;
    EXPECT_EQ(p.GetDegree(), 3) << "Degree should be 3, got " << p.GetDegree();
}

TEST(PolynomialBoundaryTest, NegativeCoefficients) {
    Polynomial<int> p{1, -2, 3, -4};
    std::string got = p.ToString();
    std::string expected = "-4x^3+3x^2-2x+1";
    EXPECT_EQ(got, expected) 
        << "Negative coefficients: expected " << expected << ", got " << got;
}

TEST(PolynomialBoundaryTest, CoefficientOne) {
    Polynomial<int> p{1, 1, 1};
    std::string got = p.ToString();
    std::string expected = "x^2+x+1";
    EXPECT_EQ(got, expected) 
        << "Coefficient 1: expected " << expected << ", got " << got;
}

TEST(PolynomialBoundaryTest, CoefficientMinusOne) {
    Polynomial<int> p{-1, -1, -1};
    std::string got = p.ToString();
    std::string expected = "-x^2-x-1";
    EXPECT_EQ(got, expected) 
        << "Coefficient -1: expected " << expected << ", got " << got;
}