#include "PolynomialWidget.hpp"
#include "PolynomialController.hpp"
#include "OperationDialog.hpp"
#include <QMessageBox>
#include <QScrollArea>
#include <QInputDialog>

PolynomialWidget::PolynomialWidget(PolynomialController* ctrl, QWidget* parent)
    : QWidget(parent), controller(ctrl) {
    setupUI();
    connect(controller, &PolynomialController::dataChanged, this, &PolynomialWidget::updateDisplay);
    connect(controller, &PolynomialController::error, this, &PolynomialWidget::onError);
    connect(controller, &PolynomialController::newPolynomialCreated, this, &PolynomialWidget::newPolynomialCreated);  // ← ДОБАВИТЬ ЭТУ СТРОКУ
    updateDisplay();
    updateOperandList();  
}

PolynomialWidget::~PolynomialWidget() {
    delete controller;
}

void PolynomialWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setMaximumHeight(80);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: #f5f5f5; }");
    
    displayLabel = new QLabel();
    displayLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; font-family: monospace; background-color: #f5f5f5; padding: 10px; }");
    displayLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    displayLabel->setWordWrap(false);
    
    scrollArea->setWidget(displayLabel);
    mainLayout->addWidget(scrollArea);
    
    infoLabel = new QLabel();
    infoLabel->setStyleSheet("QLabel { color: gray; }");
    mainLayout->addWidget(infoLabel);
    
    coeffList = new QListWidget();
    coeffList->setMaximumHeight(150);
    coeffList->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mainLayout->addWidget(coeffList);
    
    QHBoxLayout* opLayout = new QHBoxLayout();
    addBtn = new QPushButton("Сложение");
    multiplyBtn = new QPushButton("Умножение");
    scalarBtn = new QPushButton("Умножить на скаляр");
    evaluateBtn = new QPushButton("Вычислить в точке");
    composeBtn = new QPushButton("Композиция");
    
    opLayout->addWidget(addBtn);
    opLayout->addWidget(multiplyBtn);
    opLayout->addWidget(scalarBtn);
    opLayout->addWidget(evaluateBtn);
    opLayout->addWidget(composeBtn);
    mainLayout->addLayout(opLayout);
    
    QHBoxLayout* selectLayout = new QHBoxLayout();
    selectLayout->addWidget(new QLabel("Второй операнд:"));
    operandCombo = new QComboBox();
    operandCombo->setMinimumWidth(200);
    selectOperandBtn = new QPushButton("Обновить список");
    selectLayout->addWidget(operandCombo);
    selectLayout->addWidget(selectOperandBtn);
    selectLayout->addStretch();
    mainLayout->addLayout(selectLayout);
    
    connect(addBtn, &QPushButton::clicked, this, &PolynomialWidget::onAdd);
    connect(multiplyBtn, &QPushButton::clicked, this, &PolynomialWidget::onMultiply);
    connect(scalarBtn, &QPushButton::clicked, this, &PolynomialWidget::onScalar);
    connect(evaluateBtn, &QPushButton::clicked, this, &PolynomialWidget::onEvaluate);
    connect(composeBtn, &QPushButton::clicked, this, &PolynomialWidget::onCompose);
    connect(selectOperandBtn, &QPushButton::clicked, this, &PolynomialWidget::updateOperandList);
}

void PolynomialWidget::updateDisplay() {
    displayLabel->setText(controller->toString());
    
    infoLabel->setText(QString("Тип: %2")
        .arg(controller->getTypeName()));
    
    coeffList->clear();
    QVector<QString> coeffs = controller->getCoefficientsStrings();
    for (int i = 0; i < coeffs.size(); ++i) {
        coeffList->addItem(QString("a%1 = %2").arg(i).arg(coeffs[i]));
    }
    
    updateOperandList(); 
}

void PolynomialWidget::updateOperandList() {
    QVector<PolynomialController*> allControllers = PolynomialController::getAllControllers();
    
    operandCombo->clear();
    int count = 0;
    
    for (PolynomialController* ctrl : allControllers) {
        if (ctrl != controller) {  
            QString name = QString("Poly_%1 (%2)").arg(++count).arg(ctrl->getTypeName());
            operandCombo->addItem(name, QVariant::fromValue(ctrl));
        }
    }
    
    if (operandCombo->count() == 0) {
        operandCombo->addItem("Нет доступных многочленов");
        operandCombo->setEnabled(false);
    } else {
        operandCombo->setEnabled(true);
    }
}

void PolynomialWidget::onAdd() {
    if (operandCombo->count() == 0 || !operandCombo->isEnabled()) {
        onError("Нет доступных многочленов для сложения");
        return;
    }
    
    int index = operandCombo->currentIndex();
    if (index >= 0) {
        PolynomialController* other = operandCombo->currentData().value<PolynomialController*>();
        if (other) {
            int otherIndex = PolynomialController::getControllerIndex(other);
            controller->addWithIndex(otherIndex);
        }
    }
}

void PolynomialWidget::onMultiply() {
    if (operandCombo->count() == 0 || !operandCombo->isEnabled()) {
        onError("Нет доступных многочленов для умножения");
        return;
    }
    
    int index = operandCombo->currentIndex();
    if (index >= 0) {
        PolynomialController* other = operandCombo->currentData().value<PolynomialController*>();
        if (other) {
            int otherIndex = PolynomialController::getControllerIndex(other);
            controller->multiplyWithIndex(otherIndex);
        }
    }
}

void PolynomialWidget::onScalar() {
    bool ok;
    QString scalar = QInputDialog::getText(this, "Умножение на скаляр",
        "Введите скаляр:", QLineEdit::Normal, "", &ok);
    if (ok && !scalar.isEmpty()) {
        controller->multiplyByScalar(scalar);
    }
}

void PolynomialWidget::onEvaluate() {
    bool ok;
    QString x = QInputDialog::getText(this, "Вычисление значения",
        "Введите значение x:", QLineEdit::Normal, "", &ok);
    if (ok && !x.isEmpty()) {
        controller->evaluate(x);
    }
}

void PolynomialWidget::onCompose() {
    if (operandCombo->count() == 0 || !operandCombo->isEnabled()) {
        onError("Нет доступных многочленов для композиции");
        return;
    }
    
    int index = operandCombo->currentIndex();
    if (index >= 0) {
        PolynomialController* other = operandCombo->currentData().value<PolynomialController*>();
        if (other) {
            int otherIndex = PolynomialController::getControllerIndex(other);
            controller->composeWithIndex(otherIndex);
        }
    }
}

void PolynomialWidget::onSelectOperand() {
    updateOperandList();
}

void PolynomialWidget::onError(const QString& msg) {
    QMessageBox::information(this, "Результат операции", msg);
}

void PolynomialWidget::refresh() {
    updateDisplay();
}