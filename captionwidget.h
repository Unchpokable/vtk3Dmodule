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

	QAbstractItemModel* GetModel() const noexcept
	{
		return _table->model();
	}

	void Fit() const noexcept
	{
		_table->resizeColumnsToContents();
		_table->resizeRowsToContents();
	}

private:
	QTableView* _table = nullptr;
	QLabel* _captionLabel = nullptr;
	Ui::CaptionWidgetClass *_ui;

	void LoadTableStyles() const;
	void LoadGlobalStyles();
	void LoadLabelStyles() const;
};
