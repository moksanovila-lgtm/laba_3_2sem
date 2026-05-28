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
#include <variant>

QVector<PolynomialController*> PolynomialController::allControllers;

PolynomialController::PolynomialController(QObject* parent) : QObject(parent) {
    registerController(this);
    polynomial = std::monostate{};
}

PolynomialController::~PolynomialController() {
    unregisterController(this);
    
    std::visit([](auto& p) {
        using T = std::decay_t<decltype(p)>;
        if constexpr (!std::is_same_v<T, std::monostate>) {
            if (p != nullptr) {
                delete p;
            }
        }
    }, polynomial);
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
    
    auto* p = new Polynomial<ArraySequence, int>();
    for (int c : coeffs) {
        p->Append(c);
    }
    
    type = Type::TYPE_INT;
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
    
    auto* p = new Polynomial<ArraySequence, double>();
    for (double c : coeffs) {
        p->Append(c);
    }
    
    type = Type::TYPE_DOUBLE;
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
    
    auto* p = new Polynomial<ArraySequence, std::complex<double>>();
    for (const auto& c : coeffs) {
        p->Append(c);
    }
    
    type = Type::TYPE_COMPLEX;
    polynomial = p;
    emit dataChanged();
}

void PolynomialController::createMatrixPolynomial() {
    try {
        MatrixInputDialog dialog;
        if (dialog.exec() == QDialog::Accepted && !dialog.isEmpty()) {
            QList<Matrix<double>> coeffs = dialog.getMatrices();
            if (coeffs.isEmpty()) {
                emit error("Нет введённых матриц");
                return;
            }
            auto* p = new Polynomial<ArraySequence, Matrix<double>>();
            for (const auto& mat : coeffs) {
                p->Append(mat);
            }
            type = Type::TYPE_MATRIX;
            polynomial = p;
            emit dataChanged();
        }
    } catch (const std::exception& e) {
        emit error(QString("Ошибка: ") + e.what());
    } catch (...) {
        emit error("Неизвестная ошибка");
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
        if (type == Type::TYPE_INT) {
            auto* p1 = std::get<Polynomial<ArraySequence, int>*>(polynomial);
            auto* p2 = std::get<Polynomial<ArraySequence, int>*>(other->polynomial);
            auto* pResult = new Polynomial<ArraySequence, int>(p1->Add(*p2));
            result->polynomial = pResult;
        } else if (type == Type::TYPE_DOUBLE) {
            auto* p1 = std::get<Polynomial<ArraySequence, double>*>(polynomial);
            auto* p2 = std::get<Polynomial<ArraySequence, double>*>(other->polynomial);
            auto* pResult = new Polynomial<ArraySequence, double>(p1->Add(*p2));
            result->polynomial = pResult;
        } else if (type == Type::TYPE_COMPLEX) {
            auto* p1 = std::get<Polynomial<ArraySequence, std::complex<double>>*>(polynomial);
            auto* p2 = std::get<Polynomial<ArraySequence, std::complex<double>>*>(other->polynomial);
            auto* pResult = new Polynomial<ArraySequence, std::complex<double>>(p1->Add(*p2));
            result->polynomial = pResult;
        } else if (type == Type::TYPE_MATRIX) {
            auto* p1 = std::get<Polynomial<ArraySequence, Matrix<double>>*>(polynomial);
            auto* p2 = std::get<Polynomial<ArraySequence, Matrix<double>>*>(other->polynomial);
            auto* pResult = new Polynomial<ArraySequence, Matrix<double>>(p1->Add(*p2));
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
        if (type == Type::TYPE_INT) {
            auto* p1 = std::get<Polynomial<ArraySequence, int>*>(polynomial);
            auto* p2 = std::get<Polynomial<ArraySequence, int>*>(other->polynomial);
            auto* pResult = new Polynomial<ArraySequence, int>(p1->Multiply(*p2));
            result->polynomial = pResult;
        } else if (type == Type::TYPE_DOUBLE) {
            auto* p1 = std::get<Polynomial<ArraySequence, double>*>(polynomial);
            auto* p2 = std::get<Polynomial<ArraySequence, double>*>(other->polynomial);
            auto* pResult = new Polynomial<ArraySequence, double>(p1->Multiply(*p2));
            result->polynomial = pResult;
        } else if (type == Type::TYPE_COMPLEX) {
            auto* p1 = std::get<Polynomial<ArraySequence, std::complex<double>>*>(polynomial);
            auto* p2 = std::get<Polynomial<ArraySequence, std::complex<double>>*>(other->polynomial);
            auto* pResult = new Polynomial<ArraySequence, std::complex<double>>(p1->Multiply(*p2));
            result->polynomial = pResult;
        } else if (type == Type::TYPE_MATRIX) {
            auto* p1 = std::get<Polynomial<ArraySequence, Matrix<double>>*>(polynomial);
            auto* p2 = std::get<Polynomial<ArraySequence, Matrix<double>>*>(other->polynomial);
            auto* pResult = new Polynomial<ArraySequence, Matrix<double>>(p1->Multiply(*p2));
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
        if (type == Type::TYPE_INT) {
            bool ok;
            int scalar = scalarStr.toInt(&ok);
            if (!ok) {
                emit error("Некорректное целое число: " + scalarStr);
                return;
            }
            auto* p = std::get<Polynomial<ArraySequence, int>*>(polynomial);
            *p = p->MultiplyByScalar(scalar);
            emit dataChanged();
            
        } else if (type == Type::TYPE_DOUBLE) {
            bool ok;
            double scalar = scalarStr.toDouble(&ok);
            if (!ok) {
                emit error("Некорректное вещественное число: " + scalarStr);
                return;
            }
            auto* p = std::get<Polynomial<ArraySequence, double>*>(polynomial);
            *p = p->MultiplyByScalar(scalar);
            emit dataChanged();
            
        } else if (type == Type::TYPE_COMPLEX) {
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
            auto* p = std::get<Polynomial<ArraySequence, std::complex<double>>*>(polynomial);
            *p = p->MultiplyByScalar(scalar);
            emit dataChanged();
            
        } else if (type == Type::TYPE_MATRIX) {
            bool ok;
            double scalar = scalarStr.toDouble(&ok);
            if (!ok) {
                emit error("Некорректное число: " + scalarStr);
                return;
            }
            auto* p = std::get<Polynomial<ArraySequence, Matrix<double>>*>(polynomial);
            *p = p->MultiplyByScalar(Matrix<double>::Identity(2) * scalar);
            emit dataChanged();
        }
        
    } catch (const std::exception& e) {
        emit error(QString("Ошибка при умножении на скаляр: ") + e.what());
    }
}

Matrix<double> parseMatrixFromString(const QString& str) {
    QString s = str.trimmed();
    
    if (s.startsWith('[') && s.endsWith(']')) {
        s = s.mid(1, s.length() - 2);
    }
    
    QStringList rows = s.split(';');
    int size = rows.size();
    Matrix<double> result(size);
    
    for (int i = 0; i < rows.size(); ++i) {
        QString row = rows[i].trimmed();
        
        if (row.startsWith('(') && row.endsWith(')')) {
            row = row.mid(1, row.length() - 2);
        }
        
        QStringList values = row.split(',');
        
        for (int j = 0; j < values.size(); ++j) {
            bool ok;
            double val = values[j].trimmed().toDouble(&ok);
            if (!ok) {
                throw std::invalid_argument("Invalid matrix element: " + values[j].toStdString());
            }
            result(i, j) = val;
        }
    }
    
    return result;
}

void PolynomialController::evaluate(const QString& xStr) {
    QString resultStr;
    
    try {
        if (type == Type::TYPE_INT) {
            bool ok;
            int x = xStr.toInt(&ok);
            if (!ok) {
                emit error("Некорректное целое число: " + xStr);
                return;
            }
            auto* p = std::get<Polynomial<ArraySequence, int>*>(polynomial);
            int result = p->Evaluate(x);
            resultStr = QString::number(result);
            
        } else if (type == Type::TYPE_DOUBLE) {
            bool ok;
            double x = xStr.toDouble(&ok);
            if (!ok) {
                emit error("Некорректное вещественное число: " + xStr);
                return;
            }
            auto* p = std::get<Polynomial<ArraySequence, double>*>(polynomial);
            double result = p->Evaluate(x);
            resultStr = QString::number(result);
            
        } else if (type == Type::TYPE_COMPLEX) {
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
            auto* p = std::get<Polynomial<ArraySequence, std::complex<double>>*>(polynomial);
            std::complex<double> result = p->Evaluate(x);
            
            if (result.imag() == 0) {
                resultStr = QString::number(result.real());
            } else if (result.real() == 0) {
                resultStr = (result.imag() > 0 ? "" : "-") + QString::number(std::abs(result.imag())) + "i";
            } else {
                resultStr = QString::number(result.real()) + (result.imag() > 0 ? "+" : "") + 
                           QString::number(result.imag()) + "i";
            }
            
        } else if (type == Type::TYPE_MATRIX) {
            Matrix<double> X = parseMatrixFromString(xStr);
            auto* p = std::get<Polynomial<ArraySequence, Matrix<double>>*>(polynomial);
            Matrix<double> result = p->Evaluate(X);
            
            std::stringstream ss;
            ss << result;
            resultStr = QString::fromStdString(ss.str());
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
        if (type == Type::TYPE_INT) {
            auto* p1 = std::get<Polynomial<ArraySequence, int>*>(polynomial);
            auto* p2 = std::get<Polynomial<ArraySequence, int>*>(other->polynomial);
            auto* pResult = new Polynomial<ArraySequence, int>(p1->Compose(*p2));
            result->polynomial = pResult;
        } else if (type == Type::TYPE_DOUBLE) {
            auto* p1 = std::get<Polynomial<ArraySequence, double>*>(polynomial);
            auto* p2 = std::get<Polynomial<ArraySequence, double>*>(other->polynomial);
            auto* pResult = new Polynomial<ArraySequence, double>(p1->Compose(*p2));
            result->polynomial = pResult;
        } else if (type == Type::TYPE_COMPLEX) {
            auto* p1 = std::get<Polynomial<ArraySequence, std::complex<double>>*>(polynomial);
            auto* p2 = std::get<Polynomial<ArraySequence, std::complex<double>>*>(other->polynomial);
            auto* pResult = new Polynomial<ArraySequence, std::complex<double>>(p1->Compose(*p2));
            result->polynomial = pResult;
        } else if (type == Type::TYPE_MATRIX) {
            auto* p1 = std::get<Polynomial<ArraySequence, Matrix<double>>*>(polynomial);
            auto* p2 = std::get<Polynomial<ArraySequence, Matrix<double>>*>(other->polynomial);
            auto* pResult = new Polynomial<ArraySequence, Matrix<double>>(p1->Compose(*p2));
            result->polynomial = pResult;
        }
        
        QString newName = QString("%1 ∘ %2").arg(getControllerName(this)).arg(getControllerName(other));
        emit newPolynomialCreated(result, newName);
        
    } catch (const std::exception& e) {
        delete result;
        emit error(QString("Ошибка при композиции: ") + e.what());
    }
}

QString PolynomialController::toString() const {
    try {
        if (type == Type::TYPE_INT) {
            auto* p = std::get<Polynomial<ArraySequence, int>*>(polynomial);
            return p ? QString::fromStdString(p->ToString()) : "0";
        } else if (type == Type::TYPE_DOUBLE) {
            auto* p = std::get<Polynomial<ArraySequence, double>*>(polynomial);
            return p ? QString::fromStdString(p->ToString()) : "0";
        } else if (type == Type::TYPE_COMPLEX) {
            auto* p = std::get<Polynomial<ArraySequence, std::complex<double>>*>(polynomial);
            return p ? QString::fromStdString(p->ToString()) : "0";
        } else if (type == Type::TYPE_MATRIX) {
            auto* p = std::get<Polynomial<ArraySequence, Matrix<double>>*>(polynomial);
            return p ? QString::fromStdString(p->ToString()) : "0";
        }
    } catch (...) {
        return "0";
    }
    return "0";
}

int PolynomialController::getDegree() const {
    try {
        if (type == Type::TYPE_INT) {
            auto* p = std::get<Polynomial<ArraySequence, int>*>(polynomial);
            return p ? p->GetDegree() : 0;
        } else if (type == Type::TYPE_DOUBLE) {
            auto* p = std::get<Polynomial<ArraySequence, double>*>(polynomial);
            return p ? p->GetDegree() : 0;
        } else if (type == Type::TYPE_COMPLEX) {
            auto* p = std::get<Polynomial<ArraySequence, std::complex<double>>*>(polynomial);
            return p ? p->GetDegree() : 0;
        } else if (type == Type::TYPE_MATRIX) {
            auto* p = std::get<Polynomial<ArraySequence, Matrix<double>>*>(polynomial);
            return p ? p->GetDegree() : 0;
        }
    } catch (...) {
        return 0;
    }
    return 0;
}

QString PolynomialController::getTypeName() const {
    switch (type) {
        case Type::TYPE_INT: return "int";
        case Type::TYPE_DOUBLE: return "double";
        case Type::TYPE_COMPLEX: return "complex";
        case Type::TYPE_MATRIX: return "matrix";
        default: return "none";
    }
}

QVector<QString> PolynomialController::getCoefficientsStrings() const {
    QVector<QString> result;
    
    try {
        if (type == Type::TYPE_INT) {
            auto* p = std::get<Polynomial<ArraySequence, int>*>(polynomial);
            if (p) {
                for (size_t i = 0; i < p->GetCount(); ++i) {
                    result.append(QString::number(p->GetCoefficient(i)));
                }
            }
        } else if (type == Type::TYPE_DOUBLE) {
            auto* p = std::get<Polynomial<ArraySequence, double>*>(polynomial);
            if (p) {
                for (size_t i = 0; i < p->GetCount(); ++i) {
                    result.append(QString::number(p->GetCoefficient(i)));
                }
            }
        } else if (type == Type::TYPE_COMPLEX) {
            auto* p = std::get<Polynomial<ArraySequence, std::complex<double>>*>(polynomial);
            if (p) {
                for (size_t i = 0; i < p->GetCount(); ++i) {
                    std::complex<double> c = p->GetCoefficient(i);
                    if (c.imag() == 0) {
                        result.append(QString::number(c.real()));
                    } else if (c.real() == 0) {
                        result.append((c.imag() > 0 ? "" : "-") + QString::number(std::abs(c.imag())) + "i");
                    } else {
                        result.append(QString::number(c.real()) + (c.imag() > 0 ? "+" : "-") + 
                                     QString::number(std::abs(c.imag())) + "i");
                    }
                }
            }
        } else if (type == Type::TYPE_MATRIX) {
            auto* p = std::get<Polynomial<ArraySequence, Matrix<double>>*>(polynomial);
            if (p) {
                for (size_t i = 0; i < p->GetCount(); ++i) {
                    Matrix<double> mat = p->GetCoefficient(i);
                    QString matStr;
                    size_t sz = mat.GetSize();
                    for (size_t r = 0; r < sz; ++r) {
                        if (r > 0) matStr += "; ";
                        matStr += "[";
                        for (size_t c = 0; c < sz; ++c) {
                            if (c > 0) matStr += ", ";
                            matStr += QString::number(mat(r, c));
                        }
                        matStr += "]";
                    }
                    result.append(matStr);
                }
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

void PolynomialController::evaluate(const Matrix<double>& X) {
    try {
        if (type != Type::TYPE_MATRIX) {
            emit error("Многочлен не является матричным");
            return;
        }
        
        auto* p = std::get<Polynomial<ArraySequence, Matrix<double>>*>(polynomial);
        Matrix<double> result = p->Evaluate(X);
        
        std::stringstream ss;
        ss << result;
        emit error(QString("Результат вычисления: %1").arg(QString::fromStdString(ss.str())));
    } catch (const std::exception& e) {
        emit error(QString("Ошибка при вычислении: ") + e.what());
    }
}