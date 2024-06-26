#pragma once

#include <QStyledItemDelegate>

#include "captionwidget.h"

class InfoWidgetItemDelegate final : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit InfoWidgetItemDelegate(QObject *parent);

    ~InfoWidgetItemDelegate() override = default;

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /**
     * \brief Sets a new validator for output data. Validator is a function that should return int code using following rules: return < 0 - means that filtered data `lower` than should be. 0 - ok. > 0 - means that filtered data `higher` than should be
     * \param validator \c std::function that takes a \c QVariant and \c QModelIndex and returns \c int numeric code for filtered data.
     */
    void SetOutputDataValidator(const QVariantComparer& validator);

private:
    std::function<int(const QVariant&, const QModelIndex&)> _validator;

    int InvokeValidator(const QVariant& data, const QModelIndex& index) const
    {
        return _validator(data, index);
    }
};
