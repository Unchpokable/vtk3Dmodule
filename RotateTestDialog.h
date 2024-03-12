#pragma once

#include "pch.h"
#include "ui_RotateTestDialog.h"

#include "MachineHead.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class RotateTestDialogClass; };
QT_END_NAMESPACE

class RotateTestDialog : public QDialog
{
    Q_OBJECT

public:
    RotateTestDialog(QWidget* parent = nullptr, MachineHead* target = nullptr);
    ~RotateTestDialog();
    
private:
    Ui::RotateTestDialogClass *ui;
    MachineHead* _machine;

private slots:
    void Done();
};
