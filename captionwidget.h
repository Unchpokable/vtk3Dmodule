#pragma once

#include <QWidget>
#include "ui_captionwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CaptionWidgetClass; };
QT_END_NAMESPACE

using QVariantComparer = std::function<int(const QVariant&, const QModelIndex&)>;

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

	int FilterData(const QVariant& data, const QModelIndex& index) const;

	QAbstractItemModel* GetModel() const noexcept
	{
		return _table->model();
	}

	void Fit() const noexcept
	{
		_table->resizeColumnsToContents();
		_table->resizeRowsToContents();
	}

	void SetExternalFilter(const QVariantComparer& func)
	{
		if (func != nullptr)
			_externalFilter = func;
	}

private:
	QTableView* _table = nullptr;
	QLabel* _captionLabel = nullptr;
	Ui::CaptionWidgetClass *_ui;
	QVariantComparer _externalFilter;


	void LoadTableStyles() const;
	void LoadGlobalStyles();
	void LoadLabelStyles() const;
};
