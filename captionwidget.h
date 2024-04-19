#pragma once

#include <QWidget>
#include "ui_captionwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CaptionWidgetClass; };
QT_END_NAMESPACE

class CaptionWidget : public QWidget
{
	Q_OBJECT

public:
	CaptionWidget(QWidget *parent = nullptr);
	~CaptionWidget();

private:
	Ui::CaptionWidgetClass *ui;
};
