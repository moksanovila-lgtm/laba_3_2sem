#include <gtest/gtest.h>
#include "Polynomial.hpp"
#include "Matrix.hpp"
#include <sstream>

class PolynomialMatrixTest : public ::testing::Test {
protected:
    void SetUp() override {
        I = Matrix<int>::Identity(2);
        X = {
            {1, 2},
            {3, 4}
        };
        
        poly = new Polynomial<Matrix<int>>{I, I * 2, I * 3};
    }
    
    void TearDown() override {
        delete poly;
    }
    
    Matrix<int> I;
    Matrix<int> X;
    Polynomial<Matrix<int>>* poly;
};

TEST_F(PolynomialMatrixTest, Constructor) {
    Matrix<int> coeff0 = poly->GetCoefficient(0);
    Matrix<int> coeff1 = poly->GetCoefficient(1);
    Matrix<int> coeff2 = poly->GetCoefficient(2);
    EXPECT_TRUE(coeff0 == I) << "Coefficient 0 should be identity";
    EXPECT_TRUE(coeff1 == I * 2) << "Coefficient 1 should be 2I";
    EXPECT_TRUE(coeff2 == I * 3) << "Coefficient 2 should be 3I";
}

TEST_F(PolynomialMatrixTest, Evaluate) {
    Matrix<int> X2 = X * X;
    Matrix<int> expected = I + (X * 2) + (X2 * 3);
    auto result = poly->Evaluate(X);
    EXPECT_TRUE(result == expected) << "Matrix evaluate: expected " << expected << ", got " << result;
}

TEST_F(PolynomialMatrixTest, Add) {
    Polynomial<Matrix<int>> p2{I, I};
    auto result = poly->Add(p2);
    
    EXPECT_EQ(result.GetCoefficient(0), I * 2) 
        << "Add: a0 expected " << I * 2 << ", got " << result.GetCoefficient(0);
    EXPECT_EQ(result.GetCoefficient(1), I * 3) 
        << "Add: a1 expected " << I * 3 << ", got " << result.GetCoefficient(1);
    EXPECT_EQ(result.GetCoefficient(2), I * 3) 
        << "Add: a2 expected " << I * 3 << ", got " << result.GetCoefficient(2);
}

TEST_F(PolynomialMatrixTest, MultiplyByScalar) {
    auto result = poly->MultiplyByScalar(I * 2);
    EXPECT_EQ(result.GetCoefficient(0), I * 2) 
        << "MultiplyByScalar: a0 expected " << I * 2 << ", got " << result.GetCoefficient(0);
    EXPECT_EQ(result.GetCoefficient(1), I * 4) 
        << "MultiplyByScalar: a1 expected " << I * 4 << ", got " << result.GetCoefficient(1);
    EXPECT_EQ(result.GetCoefficient(2), I * 6) 
        << "MultiplyByScalar: a2 expected " << I * 6 << ", got " << result.GetCoefficient(2);
}

TEST_F(PolynomialMatrixTest, Multiply) {
    Polynomial<Matrix<int>> p1{I, I};
    Polynomial<Matrix<int>> p2{I * 2, I * 2};
    auto result = p1.Multiply(p2);
    ASSERT_EQ(result.GetCount(), 3);
    EXPECT_EQ(result.GetCoefficient(0), I * 2) 
        << "Multiply: a0 expected 2I, got " << result.GetCoefficient(0);
    EXPECT_EQ(result.GetCoefficient(1), I * 4) 
        << "Multiply: a1 expected 4I, got " << result.GetCoefficient(1);
    EXPECT_EQ(result.GetCoefficient(2), I * 2) 
        << "Multiply: a2 expected 2I, got " << result.GetCoefficient(2);
}

TEST_F(PolynomialMatrixTest, Compose) {
    Polynomial<Matrix<int>> p1{I, I};
    Polynomial<Matrix<int>> p2{I * 2, I * 2};
    auto result = p1.Compose(p2);
    ASSERT_EQ(result.GetCount(), 2);
    EXPECT_EQ(result.GetCoefficient(0), I * 3) 
        << "Compose: a0 expected 3I, got " << result.GetCoefficient(0);
    EXPECT_EQ(result.GetCoefficient(1), I * 2) 
        << "Compose: a1 expected 2I, got " << result.GetCoefficient(1);
}