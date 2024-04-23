#include "pch.h"
#include "captionwidget.h"
#include "infowidgetitemdelegate.h"

CaptionWidget::CaptionWidget(QWidget *parent)
	: QWidget(parent)
	, _ui(new Ui::CaptionWidgetClass())
{
	_ui->setupUi(this);
	_table = _ui->tableView;

	_table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	_captionLabel = _ui->label;

	const auto itemDelegate = new InfoWidgetItemDelegate(this);

    const std::function<int(const QVariant&, const QModelIndex&)> validator = [this](auto&& data, auto&& index)
    {
        return this->FilterData(std::forward<decltype(data)>(data), std::forward<decltype(index)>(index));
    };

	itemDelegate->SetOutputDataValidator(validator);

	_table->setItemDelegate(itemDelegate);

	LoadTableStyles();
	LoadGlobalStyles();
	LoadLabelStyles();
}

CaptionWidget::~CaptionWidget()
{
	delete _ui;
}

int CaptionWidget::FilterData(const QVariant& data, const QModelIndex& index) const
{
	if (_externalFilter != nullptr)
	{
		return _externalFilter(data, index);
	}

	if(index.row() == 1)
		return -1;

	if(index.row() == 2)
		return 1;

	return 0;
}

void CaptionWidget::LoadTableStyles() const
{
	QFile file("qtablewidget.qss");
	if(file.open(QFile::ReadOnly)) {
		QString styleSheet = QLatin1String(file.readAll());

		_table->setStyleSheet(styleSheet);
		file.close();
	}
}

void CaptionWidget::LoadGlobalStyles()
{
	QFile file("qwidget.qss");
	if(file.open(QFile::ReadOnly)) {
		QString styleSheet = QLatin1String(file.readAll());

		setStyleSheet(styleSheet);
		file.close();
	}
}

void CaptionWidget::LoadLabelStyles() const
{
	QFile file("qlabel.qss");
	if(file.open(QFile::ReadOnly)) {
		QString styleSheet = QLatin1String(file.readAll());

		_captionLabel->setStyleSheet(styleSheet);
		file.close();
	}
}
