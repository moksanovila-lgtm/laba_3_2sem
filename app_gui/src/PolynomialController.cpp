#include "PolynomialController.hpp"
#include "Polynomial.hpp"
#include "Matrix.hpp"
#include "MatrixInputDialog.hpp"
#include <complex>
#include <QStringList>
#include <QDebug>
#include <QDialog>
#include <sstream>
#include <iostream>

QVector<PolynomialController*> PolynomialController::allControllers;

PolynomialController::PolynomialController(QObject* parent) : QObject(parent) {
    registerController(this);
}

PolynomialController::~PolynomialController() {
    unregisterController(this);
    
    if (type == TYPE_INT) {
        delete static_cast<Polynomial<int>*>(polynomial);
    } else if (type == TYPE_DOUBLE) {
        delete static_cast<Polynomial<double>*>(polynomial);
    } else if (type == TYPE_COMPLEX) {
        delete static_cast<Polynomial<std::complex<double>>*>(polynomial);
    } else if (type == TYPE_MATRIX) {
        delete static_cast<Polynomial<Matrix<int>>*>(polynomial);
    }
    polynomial = nullptr;
}

void PolynomialController::registerController(PolynomialController* ctrl) {
    allControllers.append(ctrl);
}

void PolynomialController::unregisterController(PolynomialController* ctrl) {
    allControllers.removeAll(ctrl);
}

int PolynomialController::getControllerIndex(PolynomialController* ctrl) {
    return allControllers.indexOf(ctrl);
}

PolynomialController* PolynomialController::getControllerByIndex(int index) {
    if (index >= 0 && index < allControllers.size()) {
        return allControllers[index];
    }
    return nullptr;
}

QString PolynomialController::getControllerName(PolynomialController* ctrl) {
    int idx = getControllerIndex(ctrl);
    if (idx >= 0) {
        return QString("Poly_%1 (%2)").arg(idx + 1).arg(ctrl->getTypeName());
    }
    return "Unknown";
}

void PolynomialController::createIntPolynomial(const QString& coeffsStr) {
    QStringList parts = coeffsStr.split(' ', Qt::SkipEmptyParts);
    QList<int> coeffs;
    for (const QString& part : parts) {
        bool ok;
        int val = part.toInt(&ok);
        if (!ok) {
            emit error("Некорректное целое число: " + part);
            return;
        }
        coeffs.append(val);
    }
    
    if (coeffs.isEmpty()) {
        emit error("Не введены коэффициенты");
        return;
    }
    
    Polynomial<int>* p = new Polynomial<int>();
    for (int c : coeffs) {
        p->Append(c);
    }
    
    type = TYPE_INT;
    polynomial = p;
    emit dataChanged();
}

void PolynomialController::createDoublePolynomial(const QString& coeffsStr) {
    QStringList parts = coeffsStr.split(' ', Qt::SkipEmptyParts);
    QList<double> coeffs;
    for (const QString& part : parts) {
        bool ok;
        double val = part.toDouble(&ok);
        if (!ok) {
            emit error("Некорректное вещественное число: " + part);
            return;
        }
        coeffs.append(val);
    }
    
    if (coeffs.isEmpty()) {
        emit error("Не введены коэффициенты");
        return;
    }
    
    Polynomial<double>* p = new Polynomial<double>();
    for (double c : coeffs) {
        p->Append(c);
    }
    
    type = TYPE_DOUBLE;
    polynomial = p;
    emit dataChanged();
}

void PolynomialController::createComplexPolynomial(const QString& coeffsStr) {
    QStringList parts = coeffsStr.split(' ', Qt::SkipEmptyParts);
    QList<std::complex<double>> coeffs;
    
    for (const QString& part : parts) {
        double real = 0, imag = 0;
        QString trimmed = part.trimmed();
        
        int plusPos = trimmed.indexOf('+');
        int minusPos = trimmed.indexOf('-', 1);
        int iPos = trimmed.indexOf('i');
        
        if (iPos == -1) {
            bool ok;
            real = trimmed.toDouble(&ok);
            if (!ok) {
                emit error("Некорректное комплексное число: " + part);
                return;
            }
        } else {
            QString realPart, imagPart;
            
            if (plusPos > 0) {
                realPart = trimmed.left(plusPos);
                imagPart = trimmed.mid(plusPos + 1, iPos - plusPos - 1);
            } else if (minusPos > 0) {
                realPart = trimmed.left(minusPos);
                imagPart = trimmed.mid(minusPos, iPos - minusPos);
            } else {
                imagPart = trimmed.left(iPos);
            }
            
            if (!realPart.isEmpty()) {
                bool ok;
                real = realPart.toDouble(&ok);
                if (!ok) {
                    emit error("Некорректная действительная часть: " + realPart);
                    return;
                }
            }
            
            if (!imagPart.isEmpty()) {
                bool ok;
                imag = imagPart.toDouble(&ok);
                if (!ok) {
                    emit error("Некорректная мнимая часть: " + imagPart);
                    return;
                }
            } else if (iPos >= 0 && imagPart.isEmpty()) {
                imag = 1.0;
            }
        }
        
        coeffs.append(std::complex<double>(real, imag));
    }
    
    if (coeffs.isEmpty()) {
        emit error("Не введены коэффициенты");
        return;
    }
    
    Polynomial<std::complex<double>>* p = new Polynomial<std::complex<double>>();
    for (const auto& c : coeffs) {
        p->Append(c);
    }
    
    type = TYPE_COMPLEX;
    polynomial = p;
    emit dataChanged();
}

void PolynomialController::createMatrixPolynomial() {
    MatrixInputDialog dialog;
    if (dialog.exec() == QDialog::Accepted && !dialog.isEmpty()) {
        QList<Matrix<int>> coeffs = dialog.getMatrices();
        
        Polynomial<Matrix<int>>* p = new Polynomial<Matrix<int>>();
        for (const auto& mat : coeffs) {
            p->Append(mat);
        }
        
        type = TYPE_MATRIX;
        polynomial = p;
        emit dataChanged();
    }
}

void PolynomialController::addWithIndex(int otherIndex) {
    PolynomialController* other = getControllerByIndex(otherIndex);
    if (!other) {
        emit error("Другой многочлен не найден");
        return;
    }
    
    if (type != other->type) {
        emit error("Типы многочленов не совпадают");
        return;
    }
    
    PolynomialController* result = new PolynomialController();
    result->type = type;
    
    try {
        if (type == TYPE_INT) {
            auto* p1 = static_cast<Polynomial<int>*>(polynomial);
            auto* p2 = static_cast<Polynomial<int>*>(other->polynomial);
            auto* pResult = new Polynomial<int>(p1->Add(*p2));
            result->polynomial = pResult;
        } else if (type == TYPE_DOUBLE) {
            auto* p1 = static_cast<Polynomial<double>*>(polynomial);
            auto* p2 = static_cast<Polynomial<double>*>(other->polynomial);
            auto* pResult = new Polynomial<double>(p1->Add(*p2));
            result->polynomial = pResult;
        } else if (type == TYPE_COMPLEX) {
            auto* p1 = static_cast<Polynomial<std::complex<double>>*>(polynomial);
            auto* p2 = static_cast<Polynomial<std::complex<double>>*>(other->polynomial);
            auto* pResult = new Polynomial<std::complex<double>>(p1->Add(*p2));
            result->polynomial = pResult;
        } else if (type == TYPE_MATRIX) {
            auto* p1 = static_cast<Polynomial<Matrix<int>>*>(polynomial);
            auto* p2 = static_cast<Polynomial<Matrix<int>>*>(other->polynomial);
            auto* pResult = new Polynomial<Matrix<int>>(p1->Add(*p2));
            result->polynomial = pResult;
        }
        
        QString newName = QString("%1 + %2").arg(getControllerName(this)).arg(getControllerName(other));
        emit newPolynomialCreated(result, newName);
        
    } catch (const std::exception& e) {
        delete result;
        emit error(QString("Ошибка при сложении: ") + e.what());
    }
}

void PolynomialController::multiplyWithIndex(int otherIndex) {
    PolynomialController* other = getControllerByIndex(otherIndex);
    if (!other) {
        emit error("Другой многочлен не найден");
        return;
    }
    
    if (type != other->type) {
        emit error("Типы многочленов не совпадают");
        return;
    }
    
    PolynomialController* result = new PolynomialController();
    result->type = type;
    
    try {
        if (type == TYPE_INT) {
            auto* p1 = static_cast<Polynomial<int>*>(polynomial);
            auto* p2 = static_cast<Polynomial<int>*>(other->polynomial);
            auto* pResult = new Polynomial<int>(p1->Multiply(*p2));
            result->polynomial = pResult;
        } else if (type == TYPE_DOUBLE) {
            auto* p1 = static_cast<Polynomial<double>*>(polynomial);
            auto* p2 = static_cast<Polynomial<double>*>(other->polynomial);
            auto* pResult = new Polynomial<double>(p1->Multiply(*p2));
            result->polynomial = pResult;
        } else if (type == TYPE_COMPLEX) {
            auto* p1 = static_cast<Polynomial<std::complex<double>>*>(polynomial);
            auto* p2 = static_cast<Polynomial<std::complex<double>>*>(other->polynomial);
            auto* pResult = new Polynomial<std::complex<double>>(p1->Multiply(*p2));
            result->polynomial = pResult;
        } else if (type == TYPE_MATRIX) {
            auto* p1 = static_cast<Polynomial<Matrix<int>>*>(polynomial);
            auto* p2 = static_cast<Polynomial<Matrix<int>>*>(other->polynomial);
            auto* pResult = new Polynomial<Matrix<int>>(p1->Multiply(*p2));
            result->polynomial = pResult;
        }
        
        QString newName = QString("%1 * %2").arg(getControllerName(this)).arg(getControllerName(other));
        emit newPolynomialCreated(result, newName);
        
    } catch (const std::exception& e) {
        delete result;
        emit error(QString("Ошибка при умножении: ") + e.what());
    }
}

void PolynomialController::multiplyByScalar(const QString& scalarStr) {
    try {
        if (type == TYPE_INT) {
            bool ok;
            int scalar = scalarStr.toInt(&ok);
            if (!ok) {
                emit error("Некорректное целое число: " + scalarStr);
                return;
            }
            auto* p = static_cast<Polynomial<int>*>(polynomial);
            *p = p->MultiplyByScalar(scalar);  
            emit dataChanged(); 
            
        } else if (type == TYPE_DOUBLE) {
            bool ok;
            double scalar = scalarStr.toDouble(&ok);
            if (!ok) {
                emit error("Некорректное вещественное число: " + scalarStr);
                return;
            }
            auto* p = static_cast<Polynomial<double>*>(polynomial);
            *p = p->MultiplyByScalar(scalar);
            emit dataChanged();
            
        } else if (type == TYPE_COMPLEX) {
            double real = 0, imag = 0;
            QString trimmed = scalarStr.trimmed();
            int plusPos = trimmed.indexOf('+');
            int minusPos = trimmed.indexOf('-', 1);
            int iPos = trimmed.indexOf('i');
            
            if (iPos == -1) {
                bool ok;
                real = trimmed.toDouble(&ok);
                if (!ok) {
                    emit error("Некорректное комплексное число: " + scalarStr);
                    return;
                }
            } else {
                QString realPart, imagPart;
                if (plusPos > 0) {
                    realPart = trimmed.left(plusPos);
                    imagPart = trimmed.mid(plusPos + 1, iPos - plusPos - 1);
                } else if (minusPos > 0) {
                    realPart = trimmed.left(minusPos);
                    imagPart = trimmed.mid(minusPos, iPos - minusPos);
                } else {
                    imagPart = trimmed.left(iPos);
                }
                
                if (!realPart.isEmpty()) {
                    bool ok;
                    real = realPart.toDouble(&ok);
                    if (!ok) {
                        emit error("Некорректная действительная часть: " + realPart);
                        return;
                    }
                }
                if (!imagPart.isEmpty()) {
                    bool ok;
                    imag = imagPart.toDouble(&ok);
                    if (!ok) {
                        emit error("Некорректная мнимая часть: " + imagPart);
                        return;
                    }
                } else if (iPos >= 0 && imagPart.isEmpty()) {
                    imag = 1.0;
                }
            }
            
            std::complex<double> scalar(real, imag);
            auto* p = static_cast<Polynomial<std::complex<double>>*>(polynomial);
            *p = p->MultiplyByScalar(scalar);
            emit dataChanged();
            
        } else if (type == TYPE_MATRIX) {
            bool ok;
            int scalar = scalarStr.toInt(&ok);
            if (!ok) {
                emit error("Для матриц скаляр должен быть целым числом: " + scalarStr);
                return;
            }
            auto* p = static_cast<Polynomial<Matrix<int>>*>(polynomial);
            *p = p->MultiplyByScalar(Matrix<int>::Identity(2) * scalar);
            emit dataChanged();
        }
        
    } catch (const std::exception& e) {
        emit error(QString("Ошибка при умножении на скаляр: ") + e.what());
    }
}

void PolynomialController::evaluate(const QString& xStr) {
    QString resultStr;
    
    try {
        if (type == TYPE_INT) {
            bool ok;
            int x = xStr.toInt(&ok);
            if (!ok) {
                emit error("Некорректное целое число: " + xStr);
                return;
            }
            auto* p = static_cast<Polynomial<int>*>(polynomial);
            int result = p->Evaluate(x);
            resultStr = QString::number(result);
        } else if (type == TYPE_DOUBLE) {
            bool ok;
            double x = xStr.toDouble(&ok);
            if (!ok) {
                emit error("Некорректное вещественное число: " + xStr);
                return;
            }
            auto* p = static_cast<Polynomial<double>*>(polynomial);
            double result = p->Evaluate(x);
            resultStr = QString::number(result);
        } else if (type == TYPE_COMPLEX) {
            double real = 0, imag = 0;
            QString trimmed = xStr.trimmed();
            int plusPos = trimmed.indexOf('+');
            int minusPos = trimmed.indexOf('-', 1);
            int iPos = trimmed.indexOf('i');
            
            if (iPos == -1) {
                bool ok;
                real = trimmed.toDouble(&ok);
                if (!ok) {
                    emit error("Некорректное комплексное число: " + xStr);
                    return;
                }
            } else {
                QString realPart, imagPart;
                if (plusPos > 0) {
                    realPart = trimmed.left(plusPos);
                    imagPart = trimmed.mid(plusPos + 1, iPos - plusPos - 1);
                } else if (minusPos > 0) {
                    realPart = trimmed.left(minusPos);
                    imagPart = trimmed.mid(minusPos, iPos - minusPos);
                } else {
                    imagPart = trimmed.left(iPos);
                }
                
                if (!realPart.isEmpty()) {
                    bool ok;
                    real = realPart.toDouble(&ok);
                    if (!ok) {
                        emit error("Некорректная действительная часть: " + realPart);
                        return;
                    }
                }
                if (!imagPart.isEmpty()) {
                    bool ok;
                    imag = imagPart.toDouble(&ok);
                    if (!ok) {
                        emit error("Некорректная мнимая часть: " + imagPart);
                        return;
                    }
                } else if (iPos >= 0 && imagPart.isEmpty()) {
                    imag = 1.0;
                }
            }
            
            std::complex<double> x(real, imag);
            auto* p = static_cast<Polynomial<std::complex<double>>*>(polynomial);
            std::complex<double> result = p->Evaluate(x);
            
            if (result.imag() >= 0) {
                resultStr = QString::number(result.real()) + "+" + QString::number(result.imag()) + "i";
            } else {
                resultStr = QString::number(result.real()) + QString::number(result.imag()) + "i";
            }
        } else if (type == TYPE_MATRIX) {
            emit error("Вычисление значения для матричного многочлена требует ввода матрицы. Пока не реализовано.");
            return;
        }
        
        emit error(QString("Результат вычисления: %1").arg(resultStr));
        
    } catch (const std::exception& e) {
        emit error(QString("Ошибка при вычислении: ") + e.what());
    }
}

void PolynomialController::composeWithIndex(int otherIndex) {
    PolynomialController* other = getControllerByIndex(otherIndex);
    if (!other) {
        emit error("Другой многочлен не найден");
        return;
    }
    
    if (type != other->type) {
        emit error("Типы многочленов не совпадают");
        return;
    }
    
    PolynomialController* result = new PolynomialController();
    result->type = type;
    
    try {
        if (type == TYPE_INT) {
            auto* p1 = static_cast<Polynomial<int>*>(polynomial);
            auto* p2 = static_cast<Polynomial<int>*>(other->polynomial);
            auto* pResult = new Polynomial<int>(p1->Compose(*p2));
            result->polynomial = pResult;
        } else if (type == TYPE_DOUBLE) {
            auto* p1 = static_cast<Polynomial<double>*>(polynomial);
            auto* p2 = static_cast<Polynomial<double>*>(other->polynomial);
            auto* pResult = new Polynomial<double>(p1->Compose(*p2));
            result->polynomial = pResult;
        } else {
            delete result;
            emit error("Композиция для данного типа пока не реализована");
            return;
        }
        
        QString newName = QString("%1 ∘ %2").arg(getControllerName(this)).arg(getControllerName(other));
        emit newPolynomialCreated(result, newName);
        
    } catch (const std::exception& e) {
        delete result;
        emit error(QString("Ошибка при композиции: ") + e.what());
    }
}

QString PolynomialController::toString() const {
    if (!polynomial) return "0";
    
    try {
        if (type == TYPE_INT) {
            return QString::fromStdString(static_cast<Polynomial<int>*>(polynomial)->ToString());
        } else if (type == TYPE_DOUBLE) {
            return QString::fromStdString(static_cast<Polynomial<double>*>(polynomial)->ToString());
        } else if (type == TYPE_COMPLEX) {
            return QString::fromStdString(static_cast<Polynomial<std::complex<double>>*>(polynomial)->ToString());
        } else if (type == TYPE_MATRIX) {
            return QString::fromStdString(static_cast<Polynomial<Matrix<int>>*>(polynomial)->ToString());
        }
    } catch (...) {
        return "Error";
    }
    return "0";
}

int PolynomialController::getDegree() const {
    if (!polynomial) return 0;
    
    if (type == TYPE_INT) {
        return static_cast<Polynomial<int>*>(polynomial)->GetDegree();
    } else if (type == TYPE_DOUBLE) {
        return static_cast<Polynomial<double>*>(polynomial)->GetDegree();
    } else if (type == TYPE_COMPLEX) {
        return static_cast<Polynomial<std::complex<double>>*>(polynomial)->GetDegree();
    } else if (type == TYPE_MATRIX) {
        return static_cast<Polynomial<Matrix<int>>*>(polynomial)->GetDegree();
    }
    return 0;
}

QString PolynomialController::getTypeName() const {
    switch (type) {
        case TYPE_INT: return "int";
        case TYPE_DOUBLE: return "double";
        case TYPE_COMPLEX: return "complex";
        case TYPE_MATRIX: return "matrix";
        default: return "none";
    }
}

QVector<QString> PolynomialController::getCoefficientsStrings() const {
    QVector<QString> result;
    if (!polynomial) return result;
    
    try {
        if (type == TYPE_INT) {
            auto* p = static_cast<Polynomial<int>*>(polynomial);
            for (size_t i = 0; i < p->GetCount(); ++i) {
                result.append(QString::number(p->GetCoefficient(i)));
            }
        } else if (type == TYPE_DOUBLE) {
            auto* p = static_cast<Polynomial<double>*>(polynomial);
            for (size_t i = 0; i < p->GetCount(); ++i) {
                result.append(QString::number(p->GetCoefficient(i)));
            }
        } else if (type == TYPE_COMPLEX) {
            auto* p = static_cast<Polynomial<std::complex<double>>*>(polynomial);
            for (size_t i = 0; i < p->GetCount(); ++i) {
                std::complex<double> c = p->GetCoefficient(i);
                if (c.imag() >= 0) {
                    result.append(QString::number(c.real()) + "+" + QString::number(c.imag()) + "i");
                } else {
                    result.append(QString::number(c.real()) + QString::number(c.imag()) + "i");
                }
            }
        } else if (type == TYPE_MATRIX) {
            auto* p = static_cast<Polynomial<Matrix<int>>*>(polynomial);
            for (size_t i = 0; i < p->GetCount(); ++i) {
                result.append("[Matrix " + QString::number(i) + "]");
            }
        }
    } catch (...) {
        result.append("Error");
    }
    return result;
}

void PolynomialController::printAvailablePolynomials() const {
    QStringList list;
    for (int i = 0; i < allControllers.size(); ++i) {
        if (allControllers[i] != this) {
            list.append(getControllerName(allControllers[i]));
        }
    }
    if (list.isEmpty()) {
        const_cast<PolynomialController*>(this)->error("Нет других многочленов для операций");
    } else {
        const_cast<PolynomialController*>(this)->error("Доступные многочлены:\n" + list.join("\n"));
    }
}

void PolynomialController::setCurrentPolynomial(void* newPoly, Type newType) {
    if (type == TYPE_INT) {
        delete static_cast<Polynomial<int>*>(polynomial);
    } else if (type == TYPE_DOUBLE) {
        delete static_cast<Polynomial<double>*>(polynomial);
    } else if (type == TYPE_COMPLEX) {
        delete static_cast<Polynomial<std::complex<double>>*>(polynomial);
    } else if (type == TYPE_MATRIX) {
        delete static_cast<Polynomial<Matrix<int>>*>(polynomial);
    }
    
    polynomial = newPoly;
    type = newType;
    emit dataChanged();
}