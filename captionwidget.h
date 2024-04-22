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
	explicit CaptionWidget(QWidget *parent = nullptr);
	~CaptionWidget() override;


	void SetLabel(const QString& text) const
	{
		_captionLabel->setText(text);
	}

	void SetModel(QAbstractItemModel* model) const
    {
		_table->setModel(model);
	}

	void SetItemDelegate(QAbstractItemDelegate* delegate) const
    {
		_table->setItemDelegate(delegate);
	}

	int FilterData(const QVariant& data, const QModelIndex& index);

private:
	QTableView* _table = nullptr;
	QLabel* _captionLabel = nullptr;
	Ui::CaptionWidgetClass *ui;

	void LoadTableStyles();
	void LoadGlobalStyles();
	void LoadLabelStyles();
};
