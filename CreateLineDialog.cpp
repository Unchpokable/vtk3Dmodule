#include "pch.h"
#include "CreateLineDialog.h"
#include "EigenUtils.hpp"

CreateLineDialog::CreateLineDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateLineDialogClass())
{
    ui->setupUi(this);

    connect(ui->colorsBrowseButton, &QPushButton::clicked, this, &CreateLineDialog::OpenAndSelectColor);
    connect(ui->doneButton, &QPushButton::clicked, this, &CreateLineDialog::AcceptAndClose);
}

CreateLineDialog::~CreateLineDialog()
{
    delete ui;
}

void CreateLineDialog::OpenAndSelectColor()
{
    const auto color = QColorDialog::getColor(Qt::white, this, "Please, provide color for your line");

    if(color.isValid()) 
    {
        _lineColor = color;
        ui->colorDisplay->setAutoFillBackground(true);

        auto palette = ui->colorDisplay->palette();
        palette.setColor(QPalette::Window, _lineColor);
        ui->colorDisplay->setPalette(palette);
    }
}

void CreateLineDialog::AcceptAndClose()
{
    _data.Begin = QStringToEigenVector3d(ui->beginInput->text()).value();
    _data.End = QStringToEigenVector3d(ui->endInput->text()).value();

    _data.Color = RgbaF::FromQColor(_lineColor);

    close();
}

