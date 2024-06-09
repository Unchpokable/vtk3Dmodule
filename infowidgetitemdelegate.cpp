#include "pch.h"
#include "infowidgetitemdelegate.h"

int isNeedCheck(const QModelIndex& index)
{
    return 0;
}

InfoWidgetItemDelegate::InfoWidgetItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    
}

void InfoWidgetItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QVariant data = index.data(Qt::DisplayRole);

    QStyleOptionViewItem modifiedOption(option);


    const auto needCheck = isNeedCheck(index);
    if (!needCheck)
    {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    const auto validatorResult = InvokeValidator(data, index);

    if(validatorResult == INT_MIN || validatorResult == INT_MAX) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }
    if(validatorResult < 0)
        modifiedOption.palette.setColor(QPalette::Text, Qt::blue);

    else if(validatorResult > 0)
        modifiedOption.palette.setColor(QPalette::Text, Qt::red);

    modifiedOption.displayAlignment = Qt::AlignCenter;

    QStyledItemDelegate::paint(painter, modifiedOption, index);
}

void InfoWidgetItemDelegate::SetOutputDataValidator(const QVariantComparer& validator)
{
    if(validator == nullptr)
        throw std::invalid_argument("Given function pointer should not be a nullptr");

    _validator = validator;
}
