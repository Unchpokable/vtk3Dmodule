#include "pch.h"
#include "CreateSphereDialog.h"

#include "EigenUtils.hpp"

CreateSphereDialog::CreateSphereDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateSphereClass())
{
    ui->setupUi(this);

    connect(ui->colorBrowseButton, &QPushButton::clicked, this, &CreateSphereDialog::OpenAndSelectColor);
    connect(ui->doneButton, &QPushButton::clicked, this, &CreateSphereDialog::AcceptAndClose);
}

CreateSphereDialog::~CreateSphereDialog()
{
    delete ui;
}

void CreateSphereDialog::OpenAndSelectColor()
{
    const auto color = QColorDialog::getColor(Qt::white, this, "Please, provide color for your sphere");

    if (color.isValid())
    {
        _selectedColor = color;
        ui->colorDisplay->setAutoFillBackground(true);

        auto palette = ui->colorDisplay->palette();
        palette.setColor(QPalette::Window, _selectedColor);
        ui->colorDisplay->setPalette(palette);
    }
}

void CreateSphereDialog::ReadCenterCoordinates()
{
    _selectedPosition = ui->positionInput->text();
}

void CreateSphereDialog::ReadRadius()
{
    const auto raw = ui->radiusInput->text();
    _selectedRadius = raw.toFloat();
}

void CreateSphereDialog::AcceptAndClose()
{
    ReadCenterCoordinates();
    ReadRadius();

    _data.Color = RgbaF::FromQColor(_selectedColor);
    _data.Position = QStringToEigenVector3d(_selectedPosition).value();
    _data.Radius = _selectedRadius;

    close();
}
