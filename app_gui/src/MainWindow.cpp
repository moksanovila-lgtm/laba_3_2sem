#include "MainWindow.hpp"
#include "PolynomialWidget.hpp"
#include "PolynomialController.hpp"
#include <QMessageBox>
#include <QInputDialog>

static int polynomialCounter = 0;  

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUI();
}

void MainWindow::setupUI() {
    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    
    setWindowTitle("Polynomial Calculator - Lab 3");
    setMinimumSize(1000, 700);
    
    QHBoxLayout* createLayout = new QHBoxLayout();
    
    QLabel* typeLabel = new QLabel("Тип коэффициентов:");
    coeffTypeCombo = new QComboBox();
    coeffTypeCombo->addItem("Целые числа (int)");
    coeffTypeCombo->addItem("Вещественные числа (double)");
    coeffTypeCombo->addItem("Комплексные числа (complex)");
    coeffTypeCombo->addItem("Квадратные матрицы (Matrix)");
    
    createBtn = new QPushButton("Создать многочлен");
    createBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; }");
    
    createLayout->addWidget(typeLabel);
    createLayout->addWidget(coeffTypeCombo);
    createLayout->addWidget(createBtn);
    createLayout->addStretch();
    mainLayout->addLayout(createLayout);
    
    tabWidget = new QTabWidget();
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    mainLayout->addWidget(tabWidget);
    
    QLabel* hintLabel = new QLabel(
        "Для операций (сложение, умножение, композиция) выберите два многочлена.\n"
        "Результат откроется в новой вкладке."
    );
    hintLabel->setStyleSheet("QLabel { background-color: #f0f0f0; padding: 5px; }");
    mainLayout->addWidget(hintLabel);
    
    connect(createBtn, &QPushButton::clicked, this, &MainWindow::onCreatePolynomial);
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabClose);
}

void MainWindow::onCreatePolynomial() {
    int typeIdx = coeffTypeCombo->currentIndex();
    PolynomialController* controller = new PolynomialController();
    
    if (typeIdx == 0) {  
        bool ok;
        QString coeffsStr = QInputDialog::getText(this, "Ввод коэффициентов",
            "Введите коэффициенты (от младшего к старшему):\n"
            "Например: 1 2 3 → 1 + 2x + 3x²",
            QLineEdit::Normal, "", &ok);
        if (!ok || coeffsStr.isEmpty()) {
            delete controller;
            return;
        }
        controller->createIntPolynomial(coeffsStr);
        
    } else if (typeIdx == 1) {  
        bool ok;
        QString coeffsStr = QInputDialog::getText(this, "Ввод коэффициентов",
            "Введите коэффициенты (от младшего к старшему):\n"
            "Например: 1.5 2.5 3.5",
            QLineEdit::Normal, "", &ok);
        if (!ok || coeffsStr.isEmpty()) {
            delete controller;
            return;
        }
        controller->createDoublePolynomial(coeffsStr);
        
    } else if (typeIdx == 2) {  
        bool ok;
        QString coeffsStr = QInputDialog::getText(this, "Ввод коэффициентов",
            "Введите комплексные коэффициенты:\n"
            "Формат: 1+2i 3+4i 5+6i",
            QLineEdit::Normal, "", &ok);
        if (!ok || coeffsStr.isEmpty()) {
            delete controller;
            return;
        }
        controller->createComplexPolynomial(coeffsStr);
        
    } else {  
        controller->createMatrixPolynomial();
    }
    
    if (controller->getType() == PolynomialController::TYPE_NONE) {
        delete controller;
        QMessageBox::warning(this, "Ошибка", "Не удалось создать многочлен");
        return;
    }
    
    polynomialCounter++;
    QString tabName = QString("Poly_%1").arg(polynomialCounter);
    
    PolynomialWidget* widget = new PolynomialWidget(controller);
    tabWidget->addTab(widget, tabName);
    tabWidget->setCurrentWidget(widget);
    
    connect(widget, &PolynomialWidget::newPolynomialCreated, this, &MainWindow::addNewTab);
}

void MainWindow::onTabClose(int index) {
    QWidget* widget = tabWidget->widget(index);
    tabWidget->removeTab(index);
    delete widget;
}

void MainWindow::addNewTab(PolynomialController* controller, const QString& name) {
    polynomialCounter++;
    QString tabName = QString("Poly_%1").arg(polynomialCounter);
    
    PolynomialWidget* widget = new PolynomialWidget(controller);
    tabWidget->addTab(widget, tabName);
    tabWidget->setCurrentWidget(widget);
    connect(widget, &PolynomialWidget::newPolynomialCreated, this, &MainWindow::addNewTab);
}

QVector<PolynomialController*> MainWindow::getAllControllers() const {
    QVector<PolynomialController*> controllers;
    for (int i = 0; i < tabWidget->count(); ++i) {
        PolynomialWidget* widget = qobject_cast<PolynomialWidget*>(tabWidget->widget(i));
        if (widget && widget->getController()) {
            controllers.append(widget->getController());
        }
    }
    return controllers;
}

QString MainWindow::getControllerName(PolynomialController* controller) const {
    for (int i = 0; i < tabWidget->count(); ++i) {
        PolynomialWidget* widget = qobject_cast<PolynomialWidget*>(tabWidget->widget(i));
        if (widget && widget->getController() == controller) {
            return tabWidget->tabText(i);
        }
    }
    return "Unknown";
}