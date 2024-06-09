#include "pch.h"
#include "captionwidget.h"
#include "infowidgetitemdelegate.h"

constexpr int TableAdditionalHeight = 10;

CaptionWidget::CaptionWidget(QWidget *parent)
	: QWidget(parent)
	, _ui(new Ui::CaptionWidgetClass())
{
	_ui->setupUi(this);
	_table = _ui->tableView;

	_table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	_captionLabel = _ui->label;

	/*const auto itemDelegate = new InfoWidgetItemDelegate(this);

    const std::function<int(const QVariant&, const QModelIndex&)> validator = [this](auto&& data, auto&& index)
    {
        return this->FilterData(std::forward<decltype(data)>(data), std::forward<decltype(index)>(index));
    };

	itemDelegate->SetOutputDataValidator(validator);

	_table->setItemDelegate(itemDelegate);*/

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

	// ToDo
	if(index.row() == 1)
		return -1;

	if(index.row() == 2)
		return 1;

	return 0;
}

void CaptionWidget::Fit() noexcept
{
	if(!_table || !_captionLabel)
		return;

	_table->resizeColumnsToContents();
	_table->resizeRowsToContents();

	int tableWidth{};
	int tableHeight{};

	for(int column{}; column < _table->model()->columnCount(); ++column) {
		tableWidth += _table->columnWidth(column);
	}

	for(int row{}; row < _table->model()->rowCount(); ++row) {
		tableHeight += _table->rowHeight(row);
	}

	tableWidth += _table->verticalScrollBar()->width();
	tableHeight += _table->horizontalScrollBar()->height() + TableAdditionalHeight;

	_table->resize(tableWidth, tableHeight);

	const auto totalHeight = _captionLabel->height() + tableHeight;
	const auto totalWidth = std::max(_captionLabel->width(), tableWidth);

	resize(totalWidth, totalHeight);
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
