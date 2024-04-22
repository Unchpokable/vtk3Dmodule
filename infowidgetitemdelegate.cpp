#include "pch.h"
#include "infowidgetitemdelegate.h"

#include "captionwidget.h"

InfoWidgetItemDelegate::InfoWidgetItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    
}

InfoWidgetItemDelegate::~InfoWidgetItemDelegate()
{
    
}

void InfoWidgetItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const QVariant data = index.data(Qt::DisplayRole);

    QStyleOptionViewItem modifiedOption(option);

    const auto validatorResult = InvokeValidator(data, index);

    if(validatorResult < 0)
        modifiedOption.palette.setColor(QPalette::Text, Qt::blue);

    else if(validatorResult > 0)
        modifiedOption.palette.setColor(QPalette::Text, Qt::red);

    QStyledItemDelegate::paint(painter, modifiedOption, index);
}

void InfoWidgetItemDelegate::SetOutputDataValidator(const Validator & validator)
{
    if(validator == nullptr)
        throw std::invalid_argument("Given function pointer should not be a nullptr");

    _validator = validator;
}
