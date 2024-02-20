#pragma once

#include <QDialog>
#include "ui_CreateSphere.h"

#include "pch.h"

#include "SphereData.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class CreateSphereClass; };
QT_END_NAMESPACE


class CreateSphereDialog : public QDialog
{
    Q_OBJECT

public:
    CreateSphereDialog(QWidget *parent = nullptr);
    ~CreateSphereDialog();

    const SphereData& Data()
    {
        return _data;
    }

private:
    Ui::CreateSphereClass *ui;

    QColor _selectedColor;
    float _selectedRadius;
    QString _selectedPosition;
    void ReadCenterCoordinates();
    void ReadRadius();

    SphereData _data;

private slots:
    void OpenAndSelectColor();
    void AcceptAndClose();
};
