#include "pch.h"
#include "captionwidget.h"

CaptionWidget::CaptionWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::CaptionWidgetClass())
{
	ui->setupUi(this);
}

CaptionWidget::~CaptionWidget()
{
	delete ui;
}
