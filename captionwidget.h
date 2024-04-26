#pragma once

#include <QWidget>
#include "ui_captionwidget.h"
#include "pch.h"

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

	void setLabel(const QString& text) const
	{
		_captionLabel->setText(text);
	}

	void setModel(QAbstractItemModel* model) const
    {
		if (model)
		    _table->setModel(model);
	}

	void setItemDelegate(QAbstractItemDelegate* delegate) const
    {
		if (delegate)
		    _table->setItemDelegate(delegate);
	}

    const QLabel* getLabel() const noexcept
	{
		return _captionLabel;
	}

    QLabel* getLabel() noexcept
	{
		return _captionLabel;
	}

    const QAbstractItemModel* getModel() const noexcept
	{
		return _table->model();
	}

	QAbstractItemModel* getModel() noexcept
	{
		return _table->model();
	}

    /**
     * \brief Sets a new external data filter for widget's table ItemDelegate. 
     * \param filter \c std::function that takes \c QVariant for current obtained table cell data and \c QModelIndex given by Qt's Model of table. Return int data check code. 
     */
    void setExternalDataFilter(const QVariantComparer& filter)
	{
		if(filter)
			_externalFilter = filter;
	}

    /**
     * \brief Fits widget to its minimal size that can handle all widget visual contents without scrollbars and clipping
     */
    void fit() noexcept;

	void setExternalFilter(const QVariantComparer& func)
	{
		if (func != nullptr)
			_externalFilter = func;
	}

	void setHorizontalLabels(const QStringList& labels) const noexcept
	{
		dynamic_cast<QStandardItemModel*>(_table->model())->setHorizontalHeaderLabels(labels);
	}

	void setVerticalLabels(const QStringList& labels) const noexcept
	{
		dynamic_cast<QStandardItemModel*>(_table->model())->setVerticalHeaderLabels(labels);
	}

private:
	QTableView* _table = nullptr;
	QLabel* _captionLabel = nullptr;
	Ui::CaptionWidgetClass *_ui;
	QVariantComparer _externalFilter;

	int filterData(const QVariant& data, const QModelIndex& index) const;

	void loadTableStyles() const;
	void loadGlobalStyles();
	void loadLabelStyles() const;
};
