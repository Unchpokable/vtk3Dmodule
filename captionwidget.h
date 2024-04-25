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
		if (model)
		    _table->setModel(model);
	}

	void SetItemDelegate(QAbstractItemDelegate* delegate) const
    {
		if (delegate)
		    _table->setItemDelegate(delegate);
	}

    const QLabel* GetLabel() const noexcept
	{
		return _captionLabel;
	}

    QLabel* GetLabel() noexcept
	{
		return _captionLabel;
	}

    const QAbstractItemModel* GetModel() const noexcept
	{
		return _table->model();
	}

	QAbstractItemModel* GetModel() noexcept
	{
		return _table->model();
	}

    /**
     * \brief Sets a new external data filter for widget's table ItemDelegate. 
     * \param filter \c std::function that takes \c QVariant for current obtained table cell data and \c QModelIndex given by Qt's Model of table. Return int data check code. 
     */
    void SetExternalDataFilter(const QVariantComparer& filter)
	{
		if(filter)
			_externalFilter = filter;
	}

    /**
     * \brief Fits widget to its minimal size that can handle all widget visual contents without scrollbars and clipping
     */
    void Fit() noexcept;

	void SetExternalFilter(const QVariantComparer& func)
	{
		if (func != nullptr)
			_externalFilter = func;
	}

	void SetHorizontalLabels(const QStringList& labels) const noexcept
	{
		dynamic_cast<QStandardItemModel*>(const_cast<CaptionWidget*>(this)->GetModel())->setHorizontalHeaderLabels(labels);
	}

	void SetVerticalLabels(const QStringList& labels) const noexcept
	{
		dynamic_cast<QStandardItemModel*>(const_cast<CaptionWidget*>(this)->GetModel())->setVerticalHeaderLabels(labels);
	}

private:
	QTableView* _table = nullptr;
	QLabel* _captionLabel = nullptr;
	Ui::CaptionWidgetClass *_ui;
	QVariantComparer _externalFilter;

	int FilterData(const QVariant& data, const QModelIndex& index) const;

	void LoadTableStyles() const;
	void LoadGlobalStyles();
	void LoadLabelStyles() const;
};
