#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>   
#include <QLabel>
#include "Matrix.hpp"

class MatrixInputDialog : public QDialog {
    Q_OBJECT
    
private:
    QSpinBox* sizeSpin;
    QTableWidget* table;
    QPushButton* okBtn;
    QPushButton* cancelBtn;
    QPushButton* addMatrixBtn;
    QPushButton* clearBtn;
    
    QList<Matrix<int>> matrices;
    QList<QString> matrixPreviews;
    
    QListWidget* matrixList;
    
    void updateMatrixList();
    Matrix<int> getMatrixFromTable(int size) const;
    
public:
    MatrixInputDialog(QWidget* parent = nullptr);
    
    QList<Matrix<int>> getMatrices() const { return matrices; }
    bool isEmpty() const { return matrices.isEmpty(); }
    
private slots:
    void onSizeChanged(int size);
    void onAddMatrix();
    void onClear();
    void onRemoveMatrix(int row);
};