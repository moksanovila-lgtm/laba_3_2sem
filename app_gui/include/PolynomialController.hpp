#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <variant>
#include "Matrix.hpp"
#include "Polynomial.hpp" 

class PolynomialController : public QObject {
    Q_OBJECT
    
public:
    enum class Type {
        TYPE_NONE,
        TYPE_INT,
        TYPE_DOUBLE,
        TYPE_COMPLEX,
        TYPE_MATRIX
    };

    
private:
    Type type = Type::TYPE_NONE;
    //void* polynomial = nullptr;/// TYPE SAFETY!!!!!!!!!!
    std::variant<
    std::monostate,
    Polynomial<ArraySequence, int>*,
    Polynomial<ArraySequence, double>*,
    Polynomial<ArraySequence, std::complex<double>>*,
    Polynomial<ArraySequence, Matrix<double>>*
    > polynomial;
    
    static QVector<PolynomialController*> allControllers;
    
public:
    explicit PolynomialController(QObject* parent = nullptr);
    ~PolynomialController();
    
    static void registerController(PolynomialController* ctrl);
    static void unregisterController(PolynomialController* ctrl);  
    static int getControllerIndex(PolynomialController* ctrl);     
    static PolynomialController* getControllerByIndex(int index);
    static QString getControllerName(PolynomialController* ctrl);
    static QVector<PolynomialController*> getAllControllers() { return allControllers; }
    
    void createIntPolynomial(const QString& coeffsStr);
    void createDoublePolynomial(const QString& coeffsStr);
    void createComplexPolynomial(const QString& coeffsStr);
    void createMatrixPolynomial();
    
    void addWithIndex(int otherIndex);
    void multiplyWithIndex(int otherIndex);
    void multiplyByScalar(const QString& scalar);
    void evaluate(const QString& x);
    void evaluate(const Matrix<double>& X);
    void composeWithIndex(int otherIndex);
    
    QString toString() const;
    int getDegree() const;
    QString getTypeName() const;
    Type getType() const { return type; }
    QVector<QString> getCoefficientsStrings() const;
    
    void printAvailablePolynomials() const;
    void setCurrentPolynomial(void* newPoly, Type newType);
    
signals:
    void dataChanged();
    void error(const QString& msg);
    void newPolynomialCreated(PolynomialController* controller, const QString& name);
    void polynomialsListUpdated(const QStringList& names);
};