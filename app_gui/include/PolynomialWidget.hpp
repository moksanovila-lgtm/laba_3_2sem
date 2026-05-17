#pragma once

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>

class PolynomialController;

class PolynomialWidget : public QWidget {
    Q_OBJECT
    
private:
    PolynomialController* controller;
    
    QLabel* displayLabel;     
    QListWidget* coeffList;   
    QLabel* infoLabel;         
    
    QPushButton* addBtn;
    QPushButton* multiplyBtn;
    QPushButton* scalarBtn;
    QPushButton* evaluateBtn;
    QPushButton* composeBtn;
    
    QComboBox* operandCombo;
    QPushButton* selectOperandBtn;
    
    void setupUI();
    void updateDisplay();
    void updateOperandList();
    
private slots:
    void onAdd();
    void onMultiply();
    void onScalar();
    void onEvaluate();
    void onCompose();
    void onSelectOperand();
    void onError(const QString& msg);
    
public:
    explicit PolynomialWidget(PolynomialController* ctrl, QWidget* parent = nullptr);
    PolynomialController* getController() const { return controller; }
    ~PolynomialWidget();
    
    void refresh();
    
signals:
    void newPolynomialCreated(PolynomialController* controller, const QString& name);
    void requestPolynomialList();
};