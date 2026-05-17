#include <gtest/gtest.h>
#include "Matrix.hpp"
#include <sstream>

template <typename T>
std::string MatrixToStr(Matrix<T>& m) {
    std::stringstream ss;
    ss << m;
    return ss.str();
}

class MatrixTest : public ::testing::Test {
protected:
    void SetUp() override {
        I = Matrix<int>::Identity(2);
        A = {
            {1, 2},
            {3, 4}
        };
        B = {
            {5, 6},
            {7, 8}
        };
    }
    
    Matrix<int> I;
    Matrix<int> A;
    Matrix<int> B;
};

TEST_F(MatrixTest, ConstructorWithSize) {
    Matrix<int> m(3);
    std::string got = MatrixToStr(m);
    std::string expected = "[[0, 0, 0]; [0, 0, 0]; [0, 0, 0]]";
    EXPECT_EQ(got, expected) << "Constructor with size 3: expected " << expected << ", got " << got;
    EXPECT_EQ(m.GetSize(), 3) << "GetSize: expected 3, got " << m.GetSize();
}

TEST_F(MatrixTest, ConstructorWithInitializerList) {
    std::string got = MatrixToStr(A);
    std::string expected = "[[1, 2]; [3, 4]]";
    EXPECT_EQ(got, expected) << "Constructor with init list: expected " << expected << ", got " << got;
    EXPECT_EQ(A.GetSize(), 2) << "GetSize: expected 2, got " << A.GetSize();
}

TEST_F(MatrixTest, Identity) {
    std::string got = MatrixToStr(I);
    std::string expected = "[[1, 0]; [0, 1]]";
    EXPECT_EQ(got, expected) << "Identity(2): expected " << expected << ", got " << got;
    EXPECT_TRUE(I.IsIdentity()) << "IsIdentity should return true";
}

TEST_F(MatrixTest, OperatorParentheses) {
    EXPECT_EQ(A(0, 0), 1) << "A(0,0): expected 1, got " << A(0, 0);
    EXPECT_EQ(A(0, 1), 2) << "A(0,1): expected 2, got " << A(0, 1);
    EXPECT_EQ(A(1, 0), 3) << "A(1,0): expected 3, got " << A(1, 0);
    EXPECT_EQ(A(1, 1), 4) << "A(1,1): expected 4, got " << A(1, 1);
}

TEST_F(MatrixTest, Addition) {
    std::string before1 = MatrixToStr(A);
    std::string before2 = MatrixToStr(B);
    auto result = A + B;
    std::string got = MatrixToStr(result);
    std::string expected = "[[6, 8]; [10, 12]]";
    EXPECT_EQ(got, expected) 
        << "Addition: " << before1 << " + " << before2 << " = expected " << expected << ", got " << got;
}

TEST_F(MatrixTest, Multiplication) {
    std::string before1 = MatrixToStr(A);
    std::string before2 = MatrixToStr(B);
    auto result = A * B;
    std::string got = MatrixToStr(result);
    std::string expected = "[[19, 22]; [43, 50]]";
    EXPECT_EQ(got, expected) 
        << "Multiplication: " << before1 << " * " << before2 << " = expected " << expected << ", got " << got;
}

TEST_F(MatrixTest, MultiplyByScalar) {
    std::string before = MatrixToStr(A);
    auto result = A * 2;
    std::string got = MatrixToStr(result);
    std::string expected = "[[2, 4]; [6, 8]]";
    EXPECT_EQ(got, expected) 
        << "MultiplyByScalar(2): input=" << before << ", expected " << expected << ", got " << got;
}

TEST_F(MatrixTest, Equality) {
    Matrix<int> A_copy = {
        {1, 2},
        {3, 4}
    };
    EXPECT_TRUE(A == A_copy) << "Matrices should be equal";
    EXPECT_FALSE(A == B) << "Different matrices should not be equal";
}

TEST_F(MatrixTest, EqualityWithZero) {
    Matrix<int> zero(2);
    EXPECT_TRUE(zero == 0) << "Zero matrix should equal 0";
    EXPECT_FALSE(A == 0) << "Non-zero matrix should not equal 0";
}

TEST_F(MatrixTest, CopyConstructor) {
    Matrix<int> copy(A);
    std::string got = MatrixToStr(copy);
    std::string expected = "[[1, 2]; [3, 4]]";
    EXPECT_EQ(got, expected) << "Copy constructor: expected " << expected << ", got " << got;
    EXPECT_TRUE(copy == A) << "Copy should equal original";
}

TEST_F(MatrixTest, AssignmentOperator) {
    Matrix<int> copy;
    copy = A;
    std::string got = MatrixToStr(copy);
    std::string expected = "[[1, 2]; [3, 4]]";
    EXPECT_EQ(got, expected) << "Assignment operator: expected " << expected << ", got " << got;
    EXPECT_TRUE(copy == A) << "Assigned matrix should equal original";
}

TEST_F(MatrixTest, IsIdentity) {
    EXPECT_TRUE(I.IsIdentity()) << "Identity matrix should return true";
    EXPECT_FALSE(A.IsIdentity()) << "Non-identity matrix should return false";
}

TEST_F(MatrixTest, GetSize) {
    EXPECT_EQ(A.GetSize(), 2) << "GetSize: expected 2, got " << A.GetSize();
    Matrix<int> m3(5);
    EXPECT_EQ(m3.GetSize(), 5) << "GetSize: expected 5, got " << m3.GetSize();
}