#pragma once

#include <QDialog>
#include <QComboBox>
#include <QPushButton>

class PolynomialController;

class OperationDialog : public QDialog {
    Q_OBJECT
    
private:
    QComboBox* combo;
    QPushButton* okBtn;
    QPushButton* cancelBtn;
    
public:
    OperationDialog(const QString& title, const QStringList& items, QWidget* parent = nullptr);
    OperationDialog(const QString& title, const QVector<PolynomialController*>& controllers, QWidget* parent = nullptr);
    
    int getSelectedIndex() const;
    QString getSelectedName() const;
};