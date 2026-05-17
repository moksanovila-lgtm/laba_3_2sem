#include "OperationDialog.hpp"
#include "PolynomialController.hpp" 
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

OperationDialog::OperationDialog(const QString& title, const QStringList& items, QWidget* parent)
    : QDialog(parent) {
    
    setWindowTitle("Выбор операнда");
    setModal(true);
    setMinimumWidth(400);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QLabel* titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("QLabel { font-weight: bold; font-size: 12px; }");
    mainLayout->addWidget(titleLabel);
    
    combo = new QComboBox();
    combo->addItems(items);
    if (items.isEmpty()) {
        combo->addItem("Нет доступных многочленов");
        combo->setEnabled(false);
    }
    mainLayout->addWidget(combo);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    okBtn = new QPushButton("OK");
    okBtn->setEnabled(!items.isEmpty());
    
    cancelBtn = new QPushButton("Отмена");
    
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);
    
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

OperationDialog::OperationDialog(const QString& title, const QVector<PolynomialController*>& controllers, QWidget* parent)
    : QDialog(parent) {
    
    setWindowTitle("Выбор операнда");
    setModal(true);
    setMinimumWidth(400);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QLabel* titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("QLabel { font-weight: bold; font-size: 12px; }");
    mainLayout->addWidget(titleLabel);
    
    combo = new QComboBox();
    for (PolynomialController* ctrl : controllers) {
        if (ctrl) {
            combo->addItem(PolynomialController::getControllerName(ctrl));
        }
    }
    if (combo->count() == 0) {
        combo->addItem("Нет доступных многочленов");
        combo->setEnabled(false);
    }
    mainLayout->addWidget(combo);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    
    okBtn = new QPushButton("OK");
    okBtn->setEnabled(combo->count() > 0);
    
    cancelBtn = new QPushButton("Отмена");
    
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);
    
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

int OperationDialog::getSelectedIndex() const {
    return combo->currentIndex();
}

QString OperationDialog::getSelectedName() const {
    return combo->currentText();
}