#include "pch.h"
#include "RotateTestDialog.h"

RotateTestDialog::RotateTestDialog(QWidget *parent, MachineHead* target)
    : QDialog(parent), _machine(target)
    , ui(new Ui::RotateTestDialogClass())
{
    ui->setupUi(this);
    connect(ui->acceptButton, &QPushButton::clicked, this, &RotateTestDialog::Done);

    ui->aRotSlider->setValue(_machine->GetRotation(RotAddress::A));
    ui->bRotSlider->setValue(_machine->GetRotation(RotAddress::B));
}

RotateTestDialog::~RotateTestDialog()
{
    delete ui;
}

void RotateTestDialog::Done() 
{
    _machine->RotatePart(RotAddress::A, ui->aRotSlider->value());
    _machine->RotatePart(RotAddress::B, ui->bRotSlider->value());

    accept();
}
