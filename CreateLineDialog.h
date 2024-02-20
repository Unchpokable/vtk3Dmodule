#pragma once

#include <QDialog>

#include "LineData.hpp"
#include "ui_CreateLineDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CreateLineDialogClass; };
QT_END_NAMESPACE

class CreateLineDialog : public QDialog
{
    Q_OBJECT

public:
    CreateLineDialog(QWidget *parent = nullptr);
    ~CreateLineDialog();

    const LineData& Data() {
        return _data; 
    }

private slots:
    void OpenAndSelectColor();
    void AcceptAndClose();

private:
    QColor _lineColor;

    LineData _data;

    Ui::CreateLineDialogClass *ui;
};
