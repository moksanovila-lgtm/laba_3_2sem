#include "MatrixInputDialog.hpp"
#include <QHeaderView>
#include <QMessageBox>
#include <QLineEdit>
#include <QListWidgetItem>
#include <QDoubleSpinBox> 

MatrixInputDialog::MatrixInputDialog(QWidget* parent) 
    : QDialog(parent) {
    
    setWindowTitle("Ввод коэффициентов-матриц");
    setModal(true);
    setMinimumSize(600, 500);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QHBoxLayout* sizeLayout = new QHBoxLayout();
    sizeLayout->addWidget(new QLabel("Размер матрицы:"));
    sizeSpin = new QSpinBox();
    sizeSpin->setRange(1, 5);
    sizeSpin->setValue(2);
    sizeLayout->addWidget(sizeSpin);
    sizeLayout->addStretch();
    mainLayout->addLayout(sizeLayout);
    
    QLabel* tableLabel = new QLabel("Введите матрицу:");
    tableLabel->setStyleSheet("QLabel { font-weight: bold; margin-top: 10px; }");
    mainLayout->addWidget(tableLabel);
    
    table = new QTableWidget();
    table->setMinimumHeight(200);
    mainLayout->addWidget(table);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    addMatrixBtn = new QPushButton("Добавить матрицу в коэффициенты");
    addMatrixBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; }");
    clearBtn = new QPushButton("Очистить всё");
    
    btnLayout->addWidget(addMatrixBtn);
    btnLayout->addWidget(clearBtn);
    mainLayout->addLayout(btnLayout);
    
    QLabel* listLabel = new QLabel("Коэффициенты (от a0 до an):");
    listLabel->setStyleSheet("QLabel { font-weight: bold; margin-top: 10px; }");
    mainLayout->addWidget(listLabel);
    
    matrixList = new QListWidget();
    matrixList->setMaximumHeight(150);
    matrixList->setContextMenuPolicy(Qt::CustomContextMenu);
    mainLayout->addWidget(matrixList);
    
    QHBoxLayout* dialogBtns = new QHBoxLayout();
    okBtn = new QPushButton("OK");
    cancelBtn = new QPushButton("Отмена");
    okBtn->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font-weight: bold; }");
    dialogBtns->addStretch();
    dialogBtns->addWidget(okBtn);
    dialogBtns->addWidget(cancelBtn);
    mainLayout->addLayout(dialogBtns);
    
    connect(sizeSpin, QOverload<int>::of(&QSpinBox::valueChanged), 
            this, &MatrixInputDialog::onSizeChanged);
    connect(addMatrixBtn, &QPushButton::clicked, this, &MatrixInputDialog::onAddMatrix);
    connect(clearBtn, &QPushButton::clicked, this, &MatrixInputDialog::onClear);
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(matrixList, &QListWidget::customContextMenuRequested, 
            [this](const QPoint& pos) {
        QListWidgetItem* item = matrixList->itemAt(pos);
        if (item) {
            int row = matrixList->row(item);
            onRemoveMatrix(row);
        }
    });
    
    onSizeChanged(2);
}

void MatrixInputDialog::onSizeChanged(int size) {
    table->clear();
    table->setRowCount(size);
    table->setColumnCount(size);
    
    for (int i = 0; i < size; ++i) {
        table->setHorizontalHeaderItem(i, new QTableWidgetItem(QString::number(i)));
        table->setVerticalHeaderItem(i, new QTableWidgetItem(QString::number(i)));
    }
    
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            QLineEdit* lineEdit = new QLineEdit();
            lineEdit->setText("");  
            lineEdit->setPlaceholderText("0");  
            lineEdit->setAlignment(Qt::AlignCenter);
            table->setCellWidget(i, j, lineEdit);
        }
    }
    
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

Matrix<double> MatrixInputDialog::getMatrixFromTable(int size) const {
    Matrix<double> result(size);
    bool hasError = false;
    QString errorMsg;
    
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(table->cellWidget(i, j));
            if (lineEdit) {
                QString text = lineEdit->text().trimmed();
                double val = 0.0;
                
                if (!text.isEmpty()) {
                    QString normalized = text;
                    normalized.replace(',', '.');
                    
                    bool ok;
                    val = normalized.toDouble(&ok);
                    
                    if (!ok) {
                        hasError = true;
                        errorMsg = QString("Некорректное число в ячейке [%1][%2]: '%3'")
                                   .arg(i).arg(j).arg(text);
                        QMessageBox::warning(const_cast<MatrixInputDialog*>(this), 
                            "Ошибка ввода", errorMsg);
                        val = 0.0;
                    } else if (std::abs(val) > 1e12) {
                        QMessageBox::warning(const_cast<MatrixInputDialog*>(this), 
                            "Предупреждение", 
                            QString("Число %1 в ячейке [%2][%3] очень большое")
                            .arg(val).arg(i).arg(j));
                    }
                }
                result(i, j) = val;
            }
        }
    }
    
    return result;
}

void MatrixInputDialog::onAddMatrix() {
    int size = sizeSpin->value();
    
    bool hasError = false;
    for (int i = 0; i < size && !hasError; ++i) {
        for (int j = 0; j < size && !hasError; ++j) {
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(table->cellWidget(i, j));
            if (lineEdit) {
                QString text = lineEdit->text().trimmed();
                if (!text.isEmpty()) {
                    bool ok;
                    double val = text.toDouble(&ok);
                    if (!ok || std::abs(val) > 1e12) {
                        hasError = true;
                        QMessageBox::warning(this, "Ошибка", 
                            QString("Некорректное число в ячейке [%1][%2]: '%3'\n"
                                    "Матрица не будет добавлена.")
                            .arg(i).arg(j).arg(text));
                    }
                }
            }
        }
    }
    
    if (hasError) {
        return;  
    }
    
    Matrix<double> mat = getMatrixFromTable(size);
    
    matrices.append(mat);
    
    QString preview = "[";
    for (int i = 0; i < size; ++i) {
        if (i > 0) preview += "; ";
        preview += "[";
        for (int j = 0; j < size; ++j) {
            if (j > 0) preview += ", ";
            preview += QString::number(mat(i, j));
        }
        preview += "]";
    }
    preview += "]";
    
    matrixPreviews.append(preview);
    matrixList->addItem(QString("a%1 = %2").arg(matrices.size() - 1).arg(preview));
    
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(table->cellWidget(i, j));
            if (lineEdit) {
                lineEdit->clear();
            }
        }
    }
}

void MatrixInputDialog::onClear() {
    matrices.clear();
    matrixPreviews.clear();
    matrixList->clear();
}

void MatrixInputDialog::onRemoveMatrix(int row) {
    if (row >= 0 && row < matrices.size()) {
        matrices.removeAt(row);
        matrixPreviews.removeAt(row);
        
        matrixList->clear();
        for (int i = 0; i < matrices.size(); ++i) {
            matrixList->addItem(QString("a%1 = %2").arg(i).arg(matrixPreviews[i]));
        }
    }
}

Matrix<double> MatrixInputDialog::getSingleMatrix(QWidget* parent) {
    QDialog dialog(parent);
    dialog.setWindowTitle("Введите матрицу");
    dialog.setModal(true);
    dialog.setMinimumSize(400, 300);
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    QSpinBox* sizeSpin = new QSpinBox();
    sizeSpin->setRange(1, 5);
    sizeSpin->setValue(2);
    layout->addWidget(new QLabel("Размер матрицы:"));
    layout->addWidget(sizeSpin);
    
    QTableWidget* table = new QTableWidget();
    layout->addWidget(table);
    
    QPushButton* okBtn = new QPushButton("OK");
    QPushButton* cancelBtn = new QPushButton("Отмена");
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    layout->addLayout(btnLayout);
    
    auto updateTable = [&]() {
        int size = sizeSpin->value();
        table->clear();
        table->setRowCount(size);
        table->setColumnCount(size);
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                QTableWidgetItem* item = new QTableWidgetItem("0");
                item->setTextAlignment(Qt::AlignCenter);
                table->setItem(i, j, item);
            }
        }
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    };
    
    updateTable();
    
    QObject::connect(sizeSpin, QOverload<int>::of(&QSpinBox::valueChanged), 
        [&](int) { updateTable(); });
    QObject::connect(okBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    QObject::connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        int size = sizeSpin->value();
        Matrix<double> result(size);
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                QTableWidgetItem* item = table->item(i, j);
                if (item) {
                    result(i, j) = item->text().toDouble();
                }
            }
        }
        return result;
    }
    
    return Matrix<double>(); 
}
