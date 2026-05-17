#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class PolynomialController;

class MainWindow : public QMainWindow {
    Q_OBJECT
    
private:
    QTabWidget* tabWidget;
    QComboBox* coeffTypeCombo;
    QPushButton* createBtn;
    int polynomialCounter = 0;
    
    void setupUI();
    
private slots:
    void onCreatePolynomial();
    void onTabClose(int index);
    void addNewTab(PolynomialController* controller, const QString& name);
    void onEvaluate();
    
public:
    MainWindow(QWidget* parent = nullptr);
    QVector<PolynomialController*> getAllControllers() const;
    QString getControllerName(PolynomialController* controller) const;
};