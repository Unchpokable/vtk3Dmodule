#pragma once

#include <QStyledItemDelegate>

#include "captionwidget.h"

class InfoWidgetItemDelegate  : public QStyledItemDelegate
{
    Q_OBJECT

    using Validator = std::function<int(const QVariant&, const QModelIndex&)>;

    using NonClassValidator = std::function<int(const QVariant&)>;

public:
    explicit InfoWidgetItemDelegate(QObject *parent);

    ~InfoWidgetItemDelegate() override;

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /**
     * \brief Sets a new validator for output data. Validator is a int-return function that should return int code using following rules: return < 0 - means that filtered data `lower` than should be. 0 - ok. > 0 - means that filtered data `higher` than should be
     * \param validator std::function<int(QVariant)> that returns numeric code for filtered data.
     */
    void SetOutputDataValidator(const Validator& validator);

private:
    std::function<int(const QVariant&, const QModelIndex&)> _validator;

    int InvokeValidator(const QVariant& data, const QModelIndex& index) const
    {
        return _validator(data, index);
    }
};
