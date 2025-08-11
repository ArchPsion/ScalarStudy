#ifndef __SCALAR_WINDOW_HPP__
#define __SCALAR_WINDOW_HPP__

// Qt Libraries
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QTableView>
#include <QTextEdit>
#include <QValidator>
#include <QVBoxLayout>

// Standard Libraries
#include <array>

// Personal Libraries
#include "QAnalysingThread.hpp"
#include "QFeedingThread.hpp"
#include "QScalarModel.hpp"
#include "QTerminalWindow.hpp"

class QScalarWindow : public QMainWindow
{
	Q_OBJECT
	
	private:
	
		inline static std::vector<HexCandlestick<qreal>>				ExtractCandlesticks(std::vector<HexCandlestick<qreal>>::const_iterator, quint32, quint32);
	
		std::array<QLineEdit*, 8u>							lowEdits;
		std::array<QLineEdit*, 8u>							highEdits;
		
		QTerminalWindow* const								outputWindow = new QTerminalWindow(this);
		HexDataCenter									dataCollection;
		
		QLineEdit* const								buySuccessRateEdit = new QLineEdit(this);
		QLineEdit* const								buyAverageGainEdit = new QLineEdit(this);
		QLineEdit* const								sellSuccessRateEdit = new QLineEdit(this);
		QLineEdit* const								sellAverageGainEdit = new QLineEdit(this);
		QLineEdit* const								buyExpandedSuccessRateEdit = new QLineEdit(this);
		QLineEdit* const								buyExpandedAverageGainEdit = new QLineEdit(this);
		QLineEdit* const								sellExpandedAverageGainEdit = new QLineEdit(this);
		QLineEdit* const								sellExpandedSuccessRateEdit = new QLineEdit(this);
		
		QPushButton* const								feedButton = new QPushButton("Feed", this);
		QPushButton* const								scanButton = new QPushButton("Scan", this);
		
		QCheckBox* const								bestCheckBox = new QCheckBox("Best", this);
		QCheckBox* const								thresholdCheckBox = new QCheckBox("Threshold", this);
		QCheckBox* const								nemesisCheckBox = new QCheckBox("Nemesis", this);
		
		QComboBox* const								choiceBox = new QComboBox(this);
		
		QLineEdit* const								numberOfDaysEdit = new QLineEdit(this);
		QLineEdit* const								scopeEdit = new QLineEdit(this);
		QLineEdit* const								timeSpotEdit = new QLineEdit(this);
		QLineEdit* const								timeUnitEdit = new QLineEdit(this);
		QLineEdit* const								numberOfCandlesticksEdit = new QLineEdit(this);
		QLineEdit* const								firstCoefficientEdit = new QLineEdit(this);
		
		QLineEdit* const								bestEdit = new QLineEdit(this);
		QLineEdit* const								thresholdEdit = new QLineEdit(this);
		QLineEdit* const								balanceEdit = new QLineEdit(this);
		QLineEdit* const								gapEdit = new QLineEdit(this);
		QLineEdit* const								takeProfitEdit = new QLineEdit(this);
		QLineEdit* const								stopLossEdit = new QLineEdit(this);
		
		QLabel* const									timeUpdateLabel = new QLabel(this);
		
		QTableView* const								positiveCloneTable = new QTableView(this);
		QScalarModel									positiveModel;
		
		QTableView* const								negativeCloneTable = new QTableView(this);
		QScalarModel									negativeModel;
		
		QGraphicsView* const								feedingView = new QGraphicsView(this);
		QGraphicsScene* const								feedingScene = new QGraphicsScene(this);
		
		QGraphicsView* const								positiveView = new QGraphicsView(this);
		QGraphicsScene* const								positiveScene = new QGraphicsScene(this);
		
		QGraphicsView* const								negativeView = new QGraphicsView(this);
		QGraphicsScene* const								negativeScene = new QGraphicsScene(this);
		
		std::vector<HexCandlestick<qreal>>						candlesticksToBeAnalysed;
		QFeedingThread									feedThread;
		QAnalysingThread								scanThread;
		
		qint32										currentPositiveColumn;
		qint32										currentNegativeColumn;
		
		inline HexCheckFile								check(MissionEnum);
		inline void									recordAnalysis(void);
		inline void									search(std::vector<HexCandlestick<qreal>>::const_iterator, quint32, quint32, quint32, quint32, qreal, qreal, qreal, qreal, bool, bool);
		inline void									toggleWidgets(bool);
		inline void									updateCandlesticks(QGraphicsScene*, QGraphicsView*, const std::vector<HexCandlestick<qreal>>&, quint32, qreal) const;
		inline void									updatePanel(const HexFullReport*) const;
		inline void									updateTables(void);
		inline void									updateViews(bool) const;
	
	private slots:
	
		inline void									analyse(void);
		inline void									deleteCandlestick(void);
		inline void									feed(void);
		inline void									locate(void);
		inline void									reset(void);
		inline void									scan(void);
		inline void									showNegativeCandlesticks(const QModelIndex&);
		inline void									showPositiveCandlesticks(const QModelIndex&);
		inline void									toggleFeed(void);
		inline void									toggleScan(void);
		inline void									triggerSearch(HexUpdateFile*);
		inline void									updateGap(qreal);
		inline void									updateVolume(int);
	
	protected:
	
		inline void									closeEvent(QCloseEvent*) override;
	
	public:
	
		inline QScalarWindow(void);
};

QScalarWindow::QScalarWindow(void) :
	QMainWindow(),
	positiveModel(dataCollection.getPositivePrecedents()),
	negativeModel(dataCollection.getNegativePrecedents()),
	feedThread(dataCollection),
	scanThread(dataCollection)
{
	const auto vLayout = new QVBoxLayout(new QWidget());
	vLayout->setSpacing(0);
	vLayout->setContentsMargins(0, 0, 0, 0);
	
	QMainWindow::setCentralWidget(vLayout->parentWidget());
	QMainWindow::setWindowTitle("Scalar Study");
	QScalarWindow::choiceBox->setMaximumWidth(70);
	
	QDir dataDir("input/");
	dataDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
	dataDir.setSorting(QDir::Name);
	
	if (dataDir.exists())
	{
		const auto files = dataDir.entryList();
		
		for (const auto& str : files)
			QScalarWindow::choiceBox->addItem(str);
		
		const auto index = QScalarWindow::choiceBox->findText("MNQ");
		QScalarWindow::choiceBox->setCurrentIndex(index);
	}
	
	const auto floatValidator = new QRegularExpressionValidator(QRegularExpression("([0-9]*[.])?[0-9]+"), this);
	const auto intValidator = new QRegularExpressionValidator(QRegularExpression("([0-9]+)"), this);
	
	const auto analyseButton = new QPushButton("Analyse", this);
	const auto locateButton = new QPushButton("Locate", this);
	const auto resetButton = new QPushButton("Reset", this);
	
	const auto bList = { analyseButton, QScalarWindow::scanButton, locateButton, resetButton, QScalarWindow::feedButton };
	
	for (const auto& b : bList)
		b->setMaximumWidth(100);
	
	QObject::connect(analyseButton, SIGNAL(clicked(void)), this, SLOT(analyse(void)));
	QObject::connect(QScalarWindow::scanButton, SIGNAL(clicked(void)), this, SLOT(scan(void)));
	QObject::connect(locateButton, SIGNAL(clicked(void)), this, SLOT(locate(void)));
	QObject::connect(resetButton, SIGNAL(clicked(void)), this, SLOT(reset(void)));
	QObject::connect(QScalarWindow::feedButton, SIGNAL(clicked(void)), this, SLOT(feed(void)));
	
	QButtonGroup* group = new QButtonGroup(this);
	group->setExclusive(true);
	group->addButton(QScalarWindow::bestCheckBox);
	group->addButton(QScalarWindow::thresholdCheckBox);
	
	QScalarWindow::bestCheckBox->setMaximumWidth(55);
	QScalarWindow::thresholdCheckBox->setMaximumWidth(90);
	QScalarWindow::nemesisCheckBox->setMaximumWidth(80);
	
	const auto numberOfDaysLabel = new QLabel("Days", this);
	numberOfDaysLabel->setMaximumWidth(35);
	
	const auto scopeLabel = new QLabel("Scope", this);
	scopeLabel->setMaximumWidth(40);
	
	const auto timeSpotLabel = new QLabel("Spot", this);
	timeSpotLabel->setMaximumWidth(35);
	
	const auto timeUnitLabel = new QLabel("TU", this);
	const auto numberOfCandlesticksLabel = new QLabel("NC", this);
	const auto firstCoefficientLabel = new QLabel("FC", this);
	const auto takeProfitLabel = new QLabel("TP", this);
	const auto stopLossLabel = new QLabel("SL", this);
	
	const auto lList = { timeUnitLabel, numberOfCandlesticksLabel, firstCoefficientLabel, takeProfitLabel, stopLossLabel };
	
	for (const auto& l : lList)
		l->setMaximumWidth(25);
	
	const auto balanceLabel = new QLabel("Balance", this);
	balanceLabel->setMaximumWidth(60);
	
	const auto gapLabel = new QLabel("Gap", this);
	gapLabel->setMaximumWidth(40);
	
	QScalarWindow::numberOfDaysEdit->setValidator(intValidator);
	QScalarWindow::scopeEdit->setValidator(intValidator);
	
	QScalarWindow::timeSpotEdit->setValidator(intValidator);
	QScalarWindow::timeUnitEdit->setValidator(intValidator);
	QScalarWindow::numberOfCandlesticksEdit->setValidator(intValidator);
	QScalarWindow::firstCoefficientEdit->setValidator(floatValidator);
	
	QScalarWindow::bestEdit->setValidator(intValidator);
	QScalarWindow::thresholdEdit->setValidator(floatValidator);
	QScalarWindow::balanceEdit->setValidator(floatValidator);
	QScalarWindow::gapEdit->setValidator(floatValidator);
	
	QScalarWindow::takeProfitEdit->setValidator(floatValidator);
	QScalarWindow::stopLossEdit->setValidator(floatValidator);
	
	const auto slider = new QSlider(Qt::Horizontal, this);
	slider->setMaximumWidth(150);
	slider->setRange(0, 100);
	slider->setValue(50);
	slider->setTracking(false);
	
	const auto eList = { QScalarWindow::numberOfDaysEdit, QScalarWindow::scopeEdit,
				QScalarWindow::timeSpotEdit, QScalarWindow::timeUnitEdit,
				QScalarWindow::numberOfCandlesticksEdit, QScalarWindow::firstCoefficientEdit,
				QScalarWindow::bestEdit, QScalarWindow::thresholdEdit,
				QScalarWindow::balanceEdit, QScalarWindow::gapEdit,
				QScalarWindow::takeProfitEdit, QScalarWindow::stopLossEdit };
	
	for (const auto& e : eList)
		e->setMaximumWidth(50);
	
	const std::initializer_list<QWidget*> wList = { QScalarWindow::choiceBox, numberOfDaysLabel, QScalarWindow::numberOfDaysEdit, scopeLabel, QScalarWindow::scopeEdit,
							timeSpotLabel, QScalarWindow::timeSpotEdit, timeUnitLabel, QScalarWindow::timeUnitEdit,
							numberOfCandlesticksLabel, QScalarWindow::numberOfCandlesticksEdit,
							firstCoefficientLabel, QScalarWindow::firstCoefficientEdit,
							locateButton, analyseButton, QScalarWindow::scanButton, QScalarWindow::feedButton, resetButton,
							QScalarWindow::bestCheckBox, QScalarWindow::bestEdit, QScalarWindow::thresholdCheckBox, QScalarWindow::thresholdEdit,
							balanceLabel, QScalarWindow::balanceEdit, gapLabel, QScalarWindow::gapEdit,
							takeProfitLabel, QScalarWindow::takeProfitEdit, stopLossLabel, QScalarWindow::stopLossEdit,
							QScalarWindow::nemesisCheckBox, slider, QScalarWindow::timeUpdateLabel };
	
	const auto margin = 6;
	const auto hLayout = new QHBoxLayout(new QWidget());
	hLayout->setContentsMargins(margin, margin, margin, margin/2);
	
	for (const auto& w : wList)
		hLayout->addWidget(w);
	
	locateButton->setObjectName("Yellow");
	analyseButton->setObjectName("Yellow");
	QScalarWindow::scanButton->setObjectName("Red");
	QScalarWindow::feedButton->setObjectName("Blue");
	resetButton->setObjectName("Green");
	
	const auto centralWidget1 = hLayout->parentWidget();
	vLayout->addWidget(centralWidget1);
	
	QScalarWindow::feedingView->setScene(QScalarWindow::feedingScene);
	QScalarWindow::positiveView->setScene(QScalarWindow::positiveScene);
	QScalarWindow::negativeView->setScene(QScalarWindow::negativeScene);
	
	auto longer = false;
	
	for (const auto& view : { QScalarWindow::feedingView, QScalarWindow::positiveView, QScalarWindow::negativeView })
	{
		view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		view->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
		view->setFocusPolicy(Qt::NoFocus);
		
		view->setMinimumWidth(longer ? 600 : 500);
		view->setMinimumHeight(240);
		longer = true;
	}
	
	QScalarWindow::positiveCloneTable->setMinimumWidth(1'200);
	QScalarWindow::positiveCloneTable->setMouseTracking(true);
	
	QScalarWindow::negativeCloneTable->setMinimumWidth(1'200);
	QScalarWindow::negativeCloneTable->setMouseTracking(true);
	
	const auto gLayout = new QGridLayout(new QWidget());
	gLayout->setContentsMargins(margin, margin/2, margin, margin);
	gLayout->setColumnStretch(7, 10);
	
	for (auto i = 0; i < 7; ++i)
		gLayout->setColumnStretch(i, 0);
	
	const auto centralWidget2 = gLayout->parentWidget();
	vLayout->addWidget(centralWidget2);
	vLayout->setStretch(0, 10);
	
	gLayout->addWidget(QScalarWindow::feedingView, 0, 0, 1, 6);
	gLayout->addWidget(QScalarWindow::positiveView, 0, 6, 1, 1);
	gLayout->addWidget(QScalarWindow::negativeView, 1, 6, 8, 1);
	
	gLayout->addWidget(QScalarWindow::positiveCloneTable, 0, 7, 1, 1);
	gLayout->addWidget(QScalarWindow::negativeCloneTable, 1, 7, 8, 1);
	
	QObject::connect(QScalarWindow::positiveCloneTable, SIGNAL(entered(const QModelIndex&)), this, SLOT(showPositiveCandlesticks(const QModelIndex&)));
	QObject::connect(QScalarWindow::negativeCloneTable, SIGNAL(entered(const QModelIndex&)), this, SLOT(showNegativeCandlesticks(const QModelIndex&)));
	
	for (auto i = 0u; i < QScalarWindow::lowEdits.size(); ++i)
	{
		const auto newLabel = new QLabel("Candlestick " + QString::number(i + 1u), this);
		newLabel->setAlignment(Qt::AlignCenter);
		
		const auto newLow = new QLineEdit(this);
		newLow->setValidator(floatValidator);
		newLow->setMaximumWidth(80);
		QScalarWindow::lowEdits[i] = newLow;
		
		const auto newHigh = new QLineEdit(this);
		newHigh->setValidator(floatValidator);
		newHigh->setMaximumWidth(80);
		QScalarWindow::highEdits[i] = newHigh;
		
		const auto newButton = new QPushButton("Remove", this);
		newButton->setObjectName(QString::number(i));
		QObject::connect(newButton, SIGNAL(clicked(void)), this, SLOT(deleteCandlestick(void)));
		
		gLayout->addWidget(newLabel, i + 1, 0, 1, 1);
		gLayout->addWidget(newLow, i + 1, 1, 1, 1);
		gLayout->addWidget(newHigh, i + 1, 2, 1, 1);
		gLayout->addWidget(newButton, i + 1, 3, 1, 1);
	}
	
	const auto buySuccessRateLabel = new QLabel("Buy SR", this);
	const auto sellSuccessRateLabel = new QLabel("Sell SR", this);
	
	const auto buyAverageGainLabel = new QLabel("Buy AG", this);
	const auto sellAverageGainLabel = new QLabel("Sell AG", this);
	
	const auto buyExpandedSuccessRateLabel = new QLabel("Buy RSR", this);
	const auto sellExpandedSuccessRateLabel = new QLabel("Sell RSR", this);
	
	const auto buyExpandedAverageGainLabel = new QLabel("Buy RAG", this);
	const auto sellExpandedAverageGainLabel = new QLabel("Sell RAG", this);
	
	const std::initializer_list<QWidget*> zList = { buySuccessRateLabel, QScalarWindow::buySuccessRateEdit,
							buyAverageGainLabel, QScalarWindow::buyAverageGainEdit,
							sellSuccessRateLabel, QScalarWindow::sellSuccessRateEdit,
							sellAverageGainLabel, QScalarWindow::sellAverageGainEdit,
							buyExpandedSuccessRateLabel, QScalarWindow::buyExpandedSuccessRateEdit,
							buyExpandedAverageGainLabel, QScalarWindow::buyExpandedAverageGainEdit,
							sellExpandedSuccessRateLabel, QScalarWindow::sellExpandedSuccessRateEdit,
							sellExpandedAverageGainLabel, QScalarWindow::sellExpandedAverageGainEdit };
	
	auto count = 0;
	
	for (const auto& w : zList)
	{
		gLayout->addWidget(w, 1 + count/2, 4 + (count % 2), 1, 1);
		
		if (count % 2 == 1)
		{
			static_cast<QLineEdit*>(w)->setReadOnly(true);
			static_cast<QLineEdit*>(w)->setAlignment(Qt::AlignCenter);
			static_cast<QLineEdit*>(w)->setMaximumWidth(80);
		}
		else
		{
			static_cast<QLabel*>(w)->setAlignment(Qt::AlignCenter);
			w->setObjectName(count % 8 == 0 or count % 8 == 2 ? QString("Blue") : QString("Red"));
		}
		
		++count;
	}
	
	QScalarWindow::feedThread.setVolume(slider->value());
	
	QObject::connect(&(QScalarWindow::feedThread), SIGNAL(started(void)), this, SLOT(toggleFeed(void)));
	QObject::connect(&(QScalarWindow::feedThread), SIGNAL(finished(void)), this, SLOT(toggleFeed(void)));
	
	QObject::connect(&(QScalarWindow::feedThread), SIGNAL(triggerTableUpdate(HexUpdateFile*)), this, SLOT(triggerSearch(HexUpdateFile*)));
	QObject::connect(&(QScalarWindow::feedThread), SIGNAL(triggerGapUpdate(qreal)), this, SLOT(updateGap(qreal)));
	QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(updateVolume(int)));
	
	QObject::connect(&(QScalarWindow::scanThread), SIGNAL(started(void)), this, SLOT(toggleScan(void)));
	QObject::connect(&(QScalarWindow::scanThread), SIGNAL(finished(void)), this, SLOT(toggleScan(void)));
	
	QObject::connect(&(QScalarWindow::feedThread), SIGNAL(triggerTerminalUpdate(HexTerminalFile*)), QScalarWindow::outputWindow, SLOT(updateTable(HexTerminalFile*)));
	QObject::connect(&(QScalarWindow::scanThread), SIGNAL(triggerTerminalUpdate(HexTerminalFile*)), QScalarWindow::outputWindow, SLOT(updateTable(HexTerminalFile*)));
	
	QScalarWindow::reset();
	QMainWindow::move(QMainWindow::screen()->geometry().center() - QMainWindow::frameGeometry().center());
	
	QScalarWindow::outputWindow->setObjectName("Black");
	QScalarWindow::outputWindow->show();
	
	//const auto& screenGeometry = QGuiApplication::primaryScreen()->geometry();
	//const auto& frameGeometry = QWidget::frameGeometry();
	//QMainWindow::setGeometry((screenGeometry.width() - frameGeometry.width())/2, (screenGeometry.height() - frameGeometry.height())/2, frameGeometry.width(), frameGeometry.height);
}

void QScalarWindow::analyse(void)
{
	if (QScalarWindow::feedThread.isRunning())
		return;
	
	const auto report = QScalarWindow::check(MissionEnum::Analyse);
	
	if (report.abort)
		return;
	
	const auto invalid = not QScalarWindow::dataCollection.importDirectory(report.directory, report.numberOfDays);
	
	if (invalid)
		return;
	
	const auto now = static_cast<qreal>(report.tradeTimeSpot)/static_cast<qreal>(QScalarWindow::candlesticksToBeAnalysed.size());
	auto it = QScalarWindow::candlesticksToBeAnalysed.cbegin() + (report.tradeTimeSpot - report.timeUnit*report.numberOfCandlesticks);
	
	if (not QScalarWindow::scanThread.isRunning())
	{
		HexNormalisedInfo::UpdateCoefficients(report.firstCoefficient, report.numberOfCandlesticks);
		QScalarWindow::dataCollection.setUpHistory(report.timeUnit, report.numberOfCandlesticks, report.scope);
	}
	
	QScalarWindow::search(it, report.timeUnit, 1u, report.numberOfCandlesticks, report.numberOfCandidates, report.threshold, report.takeProfit, report.stopLoss, now, QScalarWindow::bestCheckBox->isChecked(),QScalarWindow::nemesisCheckBox->isChecked());
	QScalarWindow::timeUpdateLabel->clear();
}

HexCheckFile QScalarWindow::check(MissionEnum msn)
{
	HexCheckFile foo;
	foo.directory = QScalarWindow::choiceBox->currentText();
	foo.numberOfDays = QScalarWindow::numberOfDaysEdit->text().toUInt();
	
	if (foo.numberOfDays < 1u)
		return foo;
	
	foo.scope = QScalarWindow::scopeEdit->text().toUInt()*60u;
	
	if (foo.scope < 300u)
		return foo;
	
	foo.numberOfCandidates = QScalarWindow::bestEdit->text().toUInt();
	
	if (foo.numberOfCandidates < 5u)
		return foo;
	
	foo.threshold = QScalarWindow::thresholdEdit->text().toDouble()/100.;
	
	if (foo.threshold < 0.5 or foo.threshold >= 1.)
		return foo;
	
	foo.tradeBalance = QScalarWindow::balanceEdit->text().toDouble()/100.;
	
	if (foo.tradeBalance < 0.25 or foo.tradeBalance >= 1.)
		return foo;
	
	foo.timeUnit = QScalarWindow::timeUnitEdit->text().toUInt();
	
	if (foo.timeUnit < 1u)
		return foo;
	
	foo.numberOfCandlesticks = QScalarWindow::numberOfCandlesticksEdit->text().toUInt();
	
	if (foo.numberOfCandlesticks < 1u or foo.numberOfCandlesticks > 120u or foo.timeUnit*foo.numberOfCandlesticks > 9'999u)
		return foo;
	
	foo.firstCoefficient = QScalarWindow::firstCoefficientEdit->text().toDouble()/100.;
	
	if (foo.firstCoefficient <= 0. or foo.firstCoefficient > 1.)
		return foo;
	
	foo.takeProfit = QScalarWindow::takeProfitEdit->text().toDouble();
	foo.stopLoss = QScalarWindow::stopLossEdit->text().toDouble();
	
	if (foo.takeProfit < 0.25 or foo.stopLoss < 0.)
		return foo;
	
	if (msn == MissionEnum::Scan or msn == MissionEnum::Analyse)
	{
		foo.tradeTimeSpot = QScalarWindow::timeSpotEdit->text().toUInt();
		QScalarWindow::recordAnalysis();
	
		if (QScalarWindow::candlesticksToBeAnalysed.size() <= foo.tradeTimeSpot)
			return foo;
		
		if (msn == MissionEnum::Analyse and foo.timeUnit*foo.numberOfCandlesticks > foo.tradeTimeSpot)
			return foo;
	}
	else if (msn == MissionEnum::Feed)
	{
		const auto numberOfElementaryCandlesticks = foo.timeUnit*foo.numberOfCandlesticks;
		
		while (QFeedingThread::RealTimeCandlesticks*foo.feedingTimeUnit <= numberOfElementaryCandlesticks)
			++foo.feedingTimeUnit;
		
		if (foo.timeUnit % foo.feedingTimeUnit != 0u)
			return foo;
	}
	
	foo.abort = false;
	return foo;
}

void QScalarWindow::closeEvent(QCloseEvent*)
{
	QScalarWindow::scanThread.stop();
	QScalarWindow::feedThread.stop();
}

void QScalarWindow::deleteCandlestick(void)
{
	auto number = QObject::sender()->objectName().toUInt();
	
	for (auto i = number + 1u; i < QScalarWindow::lowEdits.size(); ++i)
	{
		const auto nextLow = QScalarWindow::lowEdits[i]->text();
		const auto nextHigh = QScalarWindow::highEdits[i]->text();
		
		if (nextLow.isEmpty() or nextHigh.isEmpty())
			continue;
		
		QScalarWindow::lowEdits[number]->setText(nextLow);
		QScalarWindow::highEdits[number]->setText(nextHigh);
		++number;
	}
	
	while (number < QScalarWindow::lowEdits.size())
	{
		QScalarWindow::lowEdits[number]->clear();
		QScalarWindow::highEdits[number]->clear();
		++number;
	}
}

std::vector<HexCandlestick<qreal>> QScalarWindow::ExtractCandlesticks(std::vector<HexCandlestick<qreal>>::const_iterator it, quint32 timeUnit, quint32 numberOfCandlesticks)
{
	std::vector<HexCandlestick<qreal>> candlesticks;
	candlesticks.reserve(numberOfCandlesticks);
	
	for (auto i = 0u; i < numberOfCandlesticks; ++i)
	{
		auto& cs = candlesticks.emplace_back(it->low, it->high);
		++it;
		
		for (auto j = 1u; j < timeUnit; ++j)
		{
			if (it->low < cs.low)
				cs.low = it->low;
			
			if (it->high > cs.high)
				cs.high = it->high;
			
			++it;
		}
	}
	
	return candlesticks;
}

void QScalarWindow::feed(void)
{
	if (QScalarWindow::scanThread.isRunning())
		return;
	
	if (QScalarWindow::feedThread.isRunning())
		return QScalarWindow::feedThread.stop();
	
	const auto report = QScalarWindow::check(MissionEnum::Feed);
	const auto gap = QScalarWindow::gapEdit->text().toDouble();
	const auto gapInvalid = QFeedingThread::CheckGap(gap);
	
	if (report.abort or gapInvalid)
		return;
	
	HexNormalisedInfo::UpdateCoefficients(report.firstCoefficient, report.numberOfCandlesticks);
	QScalarWindow::outputWindow->setUp(report.feedingTimeUnit, report.numberOfCandidates, report.threshold*100., report.takeProfit);
	
	QScalarWindow::feedThread.setUp(report, gap, QScalarWindow::bestCheckBox->isChecked(), QScalarWindow::nemesisCheckBox->isChecked());
	QScalarWindow::feedThread.start(QThread::HighPriority);
}

void QScalarWindow::locate(void)
{
	if (QScalarWindow::feedThread.isRunning())
		return;
	
	const auto report = QScalarWindow::check(MissionEnum::Locate);
	
	if (report.abort)
		return;
	
	std::vector<HexCandlestick<qreal>> candlesticks;
	candlesticks.reserve(QScalarWindow::lowEdits.size());
	
	for (auto i = 0u; i < QScalarWindow::lowEdits.size(); ++i)
	{
		const auto low = QScalarWindow::lowEdits[i]->text();
		const auto high = QScalarWindow::highEdits[i]->text();
		
		if (low.isEmpty() or high.isEmpty())
			continue;
		
		candlesticks.emplace_back(low.toDouble(), high.toDouble());
	}
	
	if (candlesticks.empty())
		return;
	
	const auto invalid = not QScalarWindow::dataCollection.importDirectory(report.directory, report.numberOfDays);
	
	if (invalid)
		return;
	
	if (not QScalarWindow::scanThread.isRunning())
	{
		HexNormalisedInfo::UpdateCoefficients(report.firstCoefficient, report.numberOfCandlesticks);
		QScalarWindow::dataCollection.setUpHistory(report.timeUnit, report.numberOfCandlesticks, report.scope);
	}
	
	QScalarWindow::search(candlesticks.cbegin(), report.timeUnit, 1u, report.numberOfCandlesticks, report.numberOfCandidates, report.threshold, report.takeProfit, report.stopLoss, 10., QScalarWindow::bestCheckBox->isChecked(), QScalarWindow::nemesisCheckBox->isChecked());
	QScalarWindow::timeUpdateLabel->clear();
}

void QScalarWindow::recordAnalysis(void)
{
	QDir analyseDirectory = QDir("analyse/");
	analyseDirectory.setNameFilters({ "*.txt" });
	analyseDirectory.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	analyseDirectory.setSorting(QDir::Name);
	
	const auto files = analyseDirectory.entryList();
	
	if (files.isEmpty())
		return;
	
	QFile candlestickFile("analyse/" + files.back());
	
	if (!candlestickFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	
	QTextStream fileReader(&candlestickFile);
	
	if (fileReader.atEnd())
		return;
	
	QScalarWindow::candlesticksToBeAnalysed.clear();
	QScalarWindow::candlesticksToBeAnalysed.reserve(23'400u);
	
	while (!fileReader.atEnd())
	{
		const auto numbers = fileReader.readLine().split(' ');
		
		if (numbers.size() == 2u)
			QScalarWindow::candlesticksToBeAnalysed.emplace_back(numbers[0u].toDouble(), numbers[1u].toDouble());
	}
}

void QScalarWindow::reset(void)
{
	QScalarWindow::numberOfDaysEdit->setText("20");
	QScalarWindow::scopeEdit->setText("400");
	
	QScalarWindow::timeSpotEdit->setText("0");
	QScalarWindow::timeUnitEdit->setText("60");
	QScalarWindow::numberOfCandlesticksEdit->setText("10");
	QScalarWindow::firstCoefficientEdit->setText("100");
	
	QScalarWindow::bestEdit->setText("10");
	QScalarWindow::thresholdEdit->setText("97");
	
	QScalarWindow::takeProfitEdit->setText("10");
	QScalarWindow::stopLossEdit->setText("15");
	
	QScalarWindow::gapEdit->setText("1");
	QScalarWindow::balanceEdit->setText("80");
	QScalarWindow::thresholdCheckBox->setChecked(true);
	
	QScalarWindow::updatePanel(nullptr);
	QScalarWindow::updateTables();
	QScalarWindow::updateViews(true);
}

void QScalarWindow::scan(void)
{
	if (QScalarWindow::feedThread.isRunning())
		return;
	
	if (QScalarWindow::scanThread.isRunning())
		return QScalarWindow::scanThread.stop();
	
	const auto report = QScalarWindow::check(MissionEnum::Scan);
	
	if (report.abort)
		return;
	
	HexNormalisedInfo::UpdateCoefficients(report.firstCoefficient, report.numberOfCandlesticks);
	QScalarWindow::outputWindow->setUp(0u, report.numberOfCandidates, report.threshold*100., report.takeProfit);
	
	QScalarWindow::scanThread.setUp(report, QScalarWindow::bestCheckBox->isChecked(), QScalarWindow::nemesisCheckBox->isChecked(), QScalarWindow::candlesticksToBeAnalysed);
	QScalarWindow::scanThread.start(QThread::HighPriority);
}

void QScalarWindow::search(std::vector<HexCandlestick<qreal>>::const_iterator it, quint32 timeUnit, quint32 feedingTimeUnit, quint32 numberOfCandlesticks, quint32 numberOfCandidates, qreal threshold, qreal takeProfit, qreal stopLoss, qreal now, bool bestOverGraduates, bool nemesis)
{
	if (bestOverGraduates)
		QScalarWindow::dataCollection.locateBest(it, timeUnit, feedingTimeUnit, numberOfCandlesticks, numberOfCandidates, now, nemesis, false);
	else
		QScalarWindow::dataCollection.locateGraduates(it, timeUnit, feedingTimeUnit, numberOfCandlesticks, threshold, now, nemesis, false);
	
	QScalarWindow::dataCollection.fullStudy(takeProfit, stopLoss);
	const auto report = QScalarWindow::dataCollection.fullReport(takeProfit, stopLoss);
	QScalarWindow::updatePanel(&report);
	
	const auto candlesticks = QScalarWindow::ExtractCandlesticks(it, timeUnit/feedingTimeUnit, numberOfCandlesticks);
	QScalarWindow::updateCandlesticks(QScalarWindow::feedingScene, QScalarWindow::feedingView, candlesticks, numberOfCandlesticks, 4.f);
	
	QScalarWindow::updateTables();
	QScalarWindow::updateViews(false);
}

void QScalarWindow::showNegativeCandlesticks(const QModelIndex& index)
{
	if (index.column() == QScalarWindow::currentNegativeColumn)
		return;
	
	const auto candlesticks = QScalarWindow::dataCollection.getCandlesticks(index.column(), false);
	const auto numberOfRealCandlesticks = QScalarWindow::numberOfCandlesticksEdit->text().toUInt();
	
	QScalarWindow::updateCandlesticks(QScalarWindow::negativeScene, QScalarWindow::negativeView, candlesticks, numberOfRealCandlesticks, 4.f);
	QScalarWindow::currentNegativeColumn = index.column();
}

void QScalarWindow::showPositiveCandlesticks(const QModelIndex& index)
{
	if (index.column() == QScalarWindow::currentPositiveColumn)
		return;
	
	const auto candlesticks = QScalarWindow::dataCollection.getCandlesticks(index.column(), true);
	const auto numberOfRealCandlesticks = QScalarWindow::numberOfCandlesticksEdit->text().toUInt();
	
	QScalarWindow::updateCandlesticks(QScalarWindow::positiveScene, QScalarWindow::positiveView, candlesticks, numberOfRealCandlesticks, 4.f);
	QScalarWindow::currentPositiveColumn = index.column();
}

void QScalarWindow::toggleFeed(void)
{
	const auto feeding = (QScalarWindow::feedButton->text() == "Feed");
	QScalarWindow::feedButton->setText(feeding ? "Feeding..." : "Feed");
	QScalarWindow::toggleWidgets(feeding);
}

void QScalarWindow::toggleScan(void)
{
	const auto scanning = (QScalarWindow::scanButton->text() == "Scan");
	QScalarWindow::scanButton->setText(scanning ? "Scanning..." : "Scan");
	QScalarWindow::toggleWidgets(scanning);
}

void QScalarWindow::toggleWidgets(bool foo)
{
	QScalarWindow::numberOfDaysEdit->setReadOnly(foo);
	QScalarWindow::scopeEdit->setReadOnly(foo);
	QScalarWindow::timeUnitEdit->setReadOnly(foo);
	QScalarWindow::numberOfCandlesticksEdit->setReadOnly(foo);
	QScalarWindow::firstCoefficientEdit->setReadOnly(foo);
	
	QScalarWindow::bestEdit->setReadOnly(foo);
	QScalarWindow::thresholdEdit->setReadOnly(foo);
	QScalarWindow::balanceEdit->setReadOnly(foo);
	QScalarWindow::gapEdit->setReadOnly(foo);
	QScalarWindow::takeProfitEdit->setReadOnly(foo);
	QScalarWindow::stopLossEdit->setReadOnly(foo);
	
	QScalarWindow::choiceBox->setEnabled(not foo);
	QScalarWindow::bestCheckBox->setEnabled(not foo);
	QScalarWindow::thresholdCheckBox->setEnabled(not foo);
	QScalarWindow::nemesisCheckBox->setEnabled(not foo);
}

void QScalarWindow::triggerSearch(HexUpdateFile* bigFile)
{
	const auto newTime = QTime::currentTime().toString("hh:mm:ss");
	QScalarWindow::timeUpdateLabel->setText("Last analysis at " + newTime + '.');
	
	QScalarWindow::search(bigFile->candlesticks.cbegin(), bigFile->timeUnit, bigFile->feedingTimeUnit, bigFile->numberOfCandlesticks, bigFile->numberOfCandidates, bigFile->threshold, bigFile->takeProfit, bigFile->stopLoss, bigFile->now, bigFile->bestOverGraduates, bigFile->nemesis);
	delete bigFile;
	
	//QWidget::showNormal();
	//QWidget::raise();
	//QWidget::activateWindow();
}

void QScalarWindow::updateCandlesticks(QGraphicsScene* scene, QGraphicsView* view, const std::vector<HexCandlestick<qreal>>& candlesticks, quint32 numberOfRealCandlesticks, qreal flat) const
{
	auto maxHeight = -std::numeric_limits<qreal>::max();
	auto minHeight = std::numeric_limits<qreal>::max();
	
	auto candlestickDrawn = 0u;
	auto x = 0.f;
	
	QPen pen(Qt::transparent);
	pen.setWidth(0);
	scene->clear();
	
	for (const auto& c : candlesticks)
	{
		const auto rect = (c.high != c.low ? QRectF(x + 0.1f, -c.high, 0.8f, c.high - c.low) : QRectF(x + 0.1f, -c.high - flat/2.f, 0.8f, flat));
		const auto item = scene->addRect(rect, pen);
		item->setBrush(candlestickDrawn < numberOfRealCandlesticks ? QColor(153, 153, 153) : QColor(204, 204, 204));
		
		if (rect.top() < minHeight)
			minHeight = rect.top();
		
		if (rect.bottom() > maxHeight)
			maxHeight = rect.bottom();
		
		x += 1.f;
		++candlestickDrawn;
	}
	
	const auto spread = maxHeight - minHeight;
	minHeight -= spread/20.f;
	maxHeight += spread/20.f;
	
	const auto entireRect = QRectF(0.f, minHeight, x, maxHeight - minHeight);
	scene->setSceneRect(entireRect);
	view->fitInView(entireRect);
	scene->update();
}

void QScalarWindow::updateGap(qreal gap)
{
	QScalarWindow::gapEdit->setText(QString::number(gap));
}

void QScalarWindow::updatePanel(const HexFullReport* report) const
{
	if (report != nullptr and QScalarWindow::positiveModel.rowCount(QModelIndex()) != 0)
	{
		const auto bSuccessRateString = QString::number(report->bSuccessRate, 'g', (report->bSuccessRate < 10. ? 3 : 4)) + '%';
		const auto sSuccessRateString = QString::number(report->sSuccessRate, 'g', (report->sSuccessRate < 10. ? 3 : 4)) + '%';
		
		QScalarWindow::buySuccessRateEdit->setText(bSuccessRateString);
		QScalarWindow::sellSuccessRateEdit->setText(sSuccessRateString);
		
		const auto bAverageGainString = QString::number(report->bAverageGain, 'g', 3);
		const auto sAverageGainString = QString::number(report->sAverageGain, 'g', 3);
		
		QScalarWindow::buyAverageGainEdit->setText(bAverageGainString);
		QScalarWindow::sellAverageGainEdit->setText(sAverageGainString);
	}
	else
	{
		QScalarWindow::buySuccessRateEdit->clear();
		QScalarWindow::sellSuccessRateEdit->clear();
		
		QScalarWindow::buyAverageGainEdit->clear();
		QScalarWindow::sellAverageGainEdit->clear();
	}
	
	if (report != nullptr and QScalarWindow::negativeModel.rowCount(QModelIndex()) != 0)
	{
		const auto bExpandedSuccessRateString = QString::number(report->bExpandedSuccessRate, 'g', (report->bExpandedSuccessRate < 10. ? 3 : 4)) + '%';
		const auto sExpandedSuccessRateString = QString::number(report->sExpandedSuccessRate, 'g', (report->sExpandedSuccessRate < 10. ? 3 : 4)) + '%';
		
		QScalarWindow::buyExpandedSuccessRateEdit->setText(bExpandedSuccessRateString);
		QScalarWindow::sellExpandedSuccessRateEdit->setText(sExpandedSuccessRateString);
		
		const auto bExpandedAverageGainString = QString::number(report->bExpandedAverageGain, 'g', 3);
		const auto sExpandedAverageGainString = QString::number(report->sExpandedAverageGain, 'g', 3);
		
		QScalarWindow::buyExpandedAverageGainEdit->setText(bExpandedAverageGainString);
		QScalarWindow::sellExpandedAverageGainEdit->setText(sExpandedAverageGainString);
	}
	else
	{
		QScalarWindow::buyExpandedSuccessRateEdit->clear();
		QScalarWindow::sellExpandedSuccessRateEdit->clear();
		
		QScalarWindow::buyExpandedAverageGainEdit->clear();
		QScalarWindow::sellExpandedAverageGainEdit->clear();
	}
}

void QScalarWindow::updateTables(void)
{
	QScalarWindow::currentPositiveColumn = -1;
	QScalarWindow::positiveCloneTable->setModel(nullptr);
	QScalarWindow::positiveCloneTable->reset();
	QScalarWindow::positiveCloneTable->setModel(&(QScalarWindow::positiveModel));
	
	QScalarWindow::currentNegativeColumn = -1;
	QScalarWindow::negativeCloneTable->setModel(nullptr);
	QScalarWindow::negativeCloneTable->reset();
	QScalarWindow::negativeCloneTable->setModel(&(QScalarWindow::negativeModel));
}

void QScalarWindow::updateViews(bool clearGeneral) const
{
	QScalarWindow::positiveScene->clear();
	QScalarWindow::positiveScene->update();
	
	QScalarWindow::negativeScene->clear();
	QScalarWindow::negativeScene->update();
	
	if (clearGeneral)
	{
		QScalarWindow::feedingScene->clear();
		QScalarWindow::feedingScene->update();
	}
}

void QScalarWindow::updateVolume(int value)
{
	const auto volume = static_cast<qreal>(value)/100.f;
	QScalarWindow::feedThread.setVolume(volume);
}

#endif
