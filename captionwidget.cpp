#include "pch.h"
#include "captionwidget.h"

#include "infowidgetitemdelegate.h"

CaptionWidget::CaptionWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::CaptionWidgetClass())
{
	ui->setupUi(this);
	_table = ui->tableView;

	_table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	_captionLabel = ui->label;

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
	delete ui;
}

int CaptionWidget::FilterData(const QVariant& data, const QModelIndex& index)
{
	if(index.row() == 1)
		return -1;

	if(index.row() == 2)
		return 1;

	return 0;
}

void CaptionWidget::LoadTableStyles()
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

void CaptionWidget::LoadLabelStyles()
{
	QFile file("qlabel.qss");
	if(file.open(QFile::ReadOnly)) {
		QString styleSheet = QLatin1String(file.readAll());

		_captionLabel->setStyleSheet(styleSheet);
		file.close();
	}
}
