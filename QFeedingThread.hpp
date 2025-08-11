#ifndef __FEEDING_THREAD_HPP__
#define __FEEDING_THREAD_HPP__

// Qt Libraries
#include <QApplication>
#include <QScreen>
#include <QSoundEffect>

// Personal Libraries
#include "QScalarThread.hpp"

class QFeedingThread : public QScalarThread
{
	Q_OBJECT
	
	private:
		
		static constexpr qint32						ChartLeftValue = 4'669;
		static constexpr qint32						ChartRightValue = 5'120;
		static constexpr qint32						ChartTopValue = 532;
		static constexpr qint32						ChartBottomValue = 1'397;
		
		static constexpr qint32						ChartWidth = ChartRightValue - ChartLeftValue;
		static constexpr qint32						ChartHeight = ChartBottomValue - ChartTopValue;
		
		static constexpr quint32						BlueColor = 4'284'914'175u;
		static constexpr quint32						RedColor =  4'294'927'974u;
		static constexpr quint32						GrayColor = 4'285'893'782u;
		
		static const std::vector<qreal>					Gaps;
		
		inline static bool						Compare(const std::vector<HexCandlestick<qreal>>&, const std::vector<HexCandlestick<qreal>>&);
		inline static std::vector<HexCandlestick<qreal>>			ExtractCandlesticks(std::vector<HexCandlestick<qreal>>::const_iterator, quint32, quint32);
		inline static void						RoundToClosestQuarter(qreal&);
		
		std::vector<HexCandlestick<qreal>>				formattedCandlesticks;
		QSoundEffect							bipTone;
		QSoundEffect							colorTone;
		
		qreal								currentGap = 1.;
		quint32								numberOfBars = 0u;
		quint32								feedingTimeUnit = 1u;
		qint32								latestColorLine = 0;
		
		inline void							runBest(void);
		inline void							runGraduates(void);
		inline bool							updateCandlesticks(void);
		inline void							updateGap(quint32);
		inline quint32							updateTerminal(const HexScanReport&, quint32, qreal) const;
	
	protected:
	
		inline void							run(void) override;
	
	public:
	
		static constexpr quint32						RealTimeCandlesticks = 300u;
		
		inline static bool						CheckGap(qreal);
	
		inline								QFeedingThread(HexDataCenter&);
		inline void							setUp(const HexCheckFile&, qreal, bool, bool);
		inline void							setVolume(qreal);
	
	signals:
	
		void								triggerGapUpdate(qreal);
		void								triggerTableUpdate(HexUpdateFile*);
};

QFeedingThread::QFeedingThread(HexDataCenter& foo) : QScalarThread(foo)
{
	QFeedingThread::formattedCandlesticks.reserve(400u);
	
	QFeedingThread::bipTone.setSource(QUrl::fromLocalFile("sounds/ring.wav"));
	QFeedingThread::colorTone.setSource(QUrl::fromLocalFile("sounds/puzzle.wav"));
}

bool QFeedingThread::CheckGap(qreal value)
{
	for (const auto& v : QFeedingThread::Gaps)
	{
		if (value == v)
			return false;
	}
	
	return true;
}

bool QFeedingThread::Compare(const std::vector<HexCandlestick<qreal>>& v1, const std::vector<HexCandlestick<qreal>>& v2)
{
	if (v1.size() != v2.size())
		return true;
	
	auto it = v1.cbegin();
	
	for (const auto& cs : v2)
	{
		if (cs.low != it->low or cs.high != it->high)
			return true;
		
		++it;
	}
	
	return false;
}

std::vector<HexCandlestick<qreal>> QFeedingThread::ExtractCandlesticks(std::vector<HexCandlestick<qreal>>::const_iterator it, quint32 timeUnit, quint32 numberOfCandlesticks)
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

void QFeedingThread::RoundToClosestQuarter(qreal& value)
{
	const auto rounded = (static_cast<qint64>(value*100. + 12.5)/25LL)*25LL;
	value = static_cast<qreal>(rounded)/100.;
}

void QFeedingThread::run(void)
{
	QScalarThread::bestOverGraduates ? QFeedingThread::runBest() : QFeedingThread::runGraduates();	
	std::cout << "Feed stopped." << std::endl;
}

void QFeedingThread::runBest(void)
{
	const auto invalid = not QScalarThread::dataCollection.importDirectory(QScalarThread::directory, QScalarThread::numberOfDays);
	
	if (invalid)
		return;
	
	QScalarThread::dataCollection.setUpHistory(QScalarThread::timeUnit, QScalarThread::numberOfCandlesticks, QScalarThread::scope);
	
	const auto numberOfElementaryCandlesticks = QScalarThread::timeUnit*QScalarThread::numberOfCandlesticks/QFeedingThread::feedingTimeUnit;
	const auto refTime = QTime(15, 30, 0);
	
	while (QScalarThread::shouldContinue)
	{
		const auto timeNow = QTime::currentTime();
		const auto secsFromRef = refTime.secsTo(timeNow);
		const auto somethingNew = QFeedingThread::updateCandlesticks();
		
		if (somethingNew)
		{
			const auto now = static_cast<qreal>(secsFromRef)/23'400.;
			const auto volatility = QScalarThread::AssessVolatility(QScalarThread::candlesticksToBeAnalysed.cend() - QScalarThread::VolatilityCandlesticks, QScalarThread::VolatilityCandlesticks);
			
			QScalarThread::dataCollection.locateBest(QScalarThread::candlesticksToBeAnalysed.cend() - numberOfElementaryCandlesticks, QScalarThread::timeUnit, QFeedingThread::feedingTimeUnit, QScalarThread::numberOfCandlesticks, QScalarThread::numberOfCandidates, now, QScalarThread::nemesis, true);
			QScalarThread::dataCollection.quickStudy(QScalarThread::stopLoss);
			
			const auto report = QScalarThread::dataCollection.scanReport(QScalarThread::takeProfit);
			const auto result = QFeedingThread::updateTerminal(report, (secsFromRef > 0 ? static_cast<quint32>(secsFromRef) : 0u), volatility);
			
			if (result == 2u)
			{
				const auto updateFile = new HexUpdateFile(QScalarThread::candlesticksToBeAnalysed, now,
										QScalarThread::timeUnit, QFeedingThread::feedingTimeUnit, QScalarThread::numberOfCandlesticks,
										QScalarThread::numberOfCandidates, QScalarThread::threshold,
										QScalarThread::takeProfit, QScalarThread::stopLoss,
										QScalarThread::bestOverGraduates, QScalarThread::nemesis);
				
				QFeedingThread::triggerTableUpdate(updateFile);
				QFeedingThread::latestColorLine = secsFromRef;
				QFeedingThread::bipTone.play();
			}
			else if (result != 0u and QFeedingThread::latestColorLine + 10 < secsFromRef)
			{
				QFeedingThread::latestColorLine = secsFromRef;
				QFeedingThread::colorTone.play();
			}
		}
		else
			QThread::msleep(50);
	}
}

void QFeedingThread::runGraduates(void)
{
	const auto invalid = not QScalarThread::dataCollection.importDirectory(QScalarThread::directory, QScalarThread::numberOfDays);
	
	if (invalid)
		return;
	
	QScalarThread::dataCollection.setUpHistory(QScalarThread::timeUnit, QScalarThread::numberOfCandlesticks, QScalarThread::scope);
	
	const auto numberOfElementaryCandlesticks = QScalarThread::timeUnit*QScalarThread::numberOfCandlesticks/QFeedingThread::feedingTimeUnit;
	const auto refTime = QTime(15, 30, 0);
	
	while (QScalarThread::shouldContinue)
	{
		const auto timeNow = QTime::currentTime();
		const auto secsFromRef = refTime.secsTo(timeNow);
		const auto somethingNew = QFeedingThread::updateCandlesticks();
		
		if (somethingNew)
		{
			const auto now = static_cast<qreal>(secsFromRef)/23'400.;
			const auto volatility = QScalarThread::AssessVolatility(QScalarThread::candlesticksToBeAnalysed.cend() - QScalarThread::VolatilityCandlesticks, QScalarThread::VolatilityCandlesticks);
			
			QScalarThread::dataCollection.locateGraduates(QScalarThread::candlesticksToBeAnalysed.cend() - numberOfElementaryCandlesticks, QScalarThread::timeUnit, QFeedingThread::feedingTimeUnit, QScalarThread::numberOfCandlesticks, QScalarThread::threshold, now, QScalarThread::nemesis, true);
			QScalarThread::dataCollection.quickStudy(QScalarThread::stopLoss);
			
			const auto report = QScalarThread::dataCollection.scanReport(QScalarThread::takeProfit);
			const auto result = QFeedingThread::updateTerminal(report, (secsFromRef > 0 ? static_cast<quint32>(secsFromRef) : 0u), volatility);
			
			if (result == 2u)
			{
				const auto updateFile = new HexUpdateFile(QScalarThread::candlesticksToBeAnalysed, now,
										QScalarThread::timeUnit, QFeedingThread::feedingTimeUnit, QScalarThread::numberOfCandlesticks,
										QScalarThread::numberOfCandidates, QScalarThread::threshold,
										QScalarThread::takeProfit, QScalarThread::stopLoss,
										QScalarThread::bestOverGraduates, QScalarThread::nemesis);
				
				QFeedingThread::triggerTableUpdate(updateFile);
				QFeedingThread::latestColorLine = secsFromRef;
				QFeedingThread::bipTone.play();
			}
			else if (result != 0u and QFeedingThread::latestColorLine + 10 < secsFromRef)
			{
				QFeedingThread::latestColorLine = secsFromRef;
				QFeedingThread::colorTone.play();
			}
		}
		else
			QThread::msleep(50);
	}
}

void QFeedingThread::setUp(const HexCheckFile& report, qreal scaleGap, bool best, bool nms)
{
	QScalarThread::generalSetUp(report, best, nms);
	QFeedingThread::currentGap = scaleGap;
	QFeedingThread::feedingTimeUnit = report.feedingTimeUnit;
	QFeedingThread::latestColorLine = 0;
}

void QFeedingThread::setVolume(qreal volume)
{
	QFeedingThread::bipTone.setVolume(volume);
	QFeedingThread::colorTone.setVolume(volume);
}

bool QFeedingThread::updateCandlesticks(void)
{
	const auto img = QGuiApplication::primaryScreen()->grabWindow(0, QFeedingThread::ChartLeftValue, QFeedingThread::ChartTopValue, QFeedingThread::ChartWidth, QFeedingThread::ChartHeight).toImage();
	auto it = reinterpret_cast<const quint32*>(img.constScanLine(0));
	auto realWidth = 0;
	
	while (*it != QFeedingThread::GrayColor and realWidth < QFeedingThread::ChartWidth)
	{
		++it;
		++realWidth;
	}
	
	const auto realImage = img.copy(0, 0, realWidth + 2, QFeedingThread::ChartHeight).transformed(QTransform().rotate(90.f));
	auto pixelIt = reinterpret_cast<const quint32*>(realImage.constScanLine(realWidth + 1));
	
	auto numberOfGrayBars = 0u;
	auto pMin = 0;
	auto pMax = 0;
	
	for (auto pixelCount = 0; pixelCount < QFeedingThread::ChartHeight; ++pixelCount)
	{
		if (*pixelIt == QFeedingThread::GrayColor)
		{
			if (numberOfGrayBars == 0u)
				pMin = pixelCount;
			
			pMax = pixelCount;
			++numberOfGrayBars;
		}
		
		++pixelIt;
	}
	
	if (numberOfGrayBars < 2u)
		return false;
	else if (QFeedingThread::numberOfBars == 0u)
		QFeedingThread::numberOfBars = numberOfGrayBars;
	
	QFeedingThread::updateGap(numberOfGrayBars);
	QFeedingThread::numberOfBars = numberOfGrayBars;
	
	const auto coeff = QFeedingThread::currentGap*static_cast<qreal>(numberOfGrayBars - 1u)/static_cast<qreal>(pMax - pMin);
	const auto numberOfElementaryCandlesticks = QScalarThread::timeUnit*QScalarThread::numberOfCandlesticks/QFeedingThread::feedingTimeUnit;
	
	std::vector<HexCandlestick<qreal>> pixelCandlesticks;
	pixelCandlesticks.reserve(500u);
	
	for (auto lineCount = 0; lineCount < realWidth; ++lineCount)
	{
		auto pxIt = reinterpret_cast<const quint32*>(realImage.constScanLine(lineCount));
		auto min = -1000.;
		auto max = -1000.;
		
		for (auto pixelCount = 0; pixelCount < QFeedingThread::ChartHeight; ++pixelCount)
		{
			switch (*pxIt)
			{
				case QFeedingThread::BlueColor:
				{
					max = coeff*static_cast<qreal>(pixelCount - pMin);
					break;
				}
				
				case QFeedingThread::RedColor:
				{
					if (min == -1000.)
						min = coeff*static_cast<qreal>(pixelCount - pMin);
					
					break;
				}
				
				default:
					break;
			}
			
			++pxIt;
		}
		
		if (max != -1000.)
		{
			if (min == -1000.)
				min = max;
			
			QFeedingThread::RoundToClosestQuarter(min);
			QFeedingThread::RoundToClosestQuarter(max);
			pixelCandlesticks.emplace_back(min, max);
		}
	}
	
	if (pixelCandlesticks.size() < numberOfElementaryCandlesticks)
		return false;
	
	auto finalCandlesticks = QFeedingThread::ExtractCandlesticks(pixelCandlesticks.cend() - numberOfElementaryCandlesticks, QScalarThread::timeUnit/QFeedingThread::feedingTimeUnit, QScalarThread::numberOfCandlesticks);
	const auto somethingChanged = QFeedingThread::Compare(finalCandlesticks, QFeedingThread::formattedCandlesticks);
	
	if (somethingChanged)
	{
		QScalarThread::candlesticksToBeAnalysed.swap(pixelCandlesticks);
		QFeedingThread::formattedCandlesticks.swap(finalCandlesticks);
	}
	
	return somethingChanged;
}

void QFeedingThread::updateGap(quint32 numberOfGrayBars)
{
	if (numberOfGrayBars + 5u < QFeedingThread::numberOfBars)
	{
		if (QFeedingThread::Gaps.back() != QFeedingThread::currentGap)
		{
			auto it = QFeedingThread::Gaps.cbegin();
			
			while (QFeedingThread::currentGap != *it)
				++it;
			
			++it;
			QFeedingThread::currentGap = *it;
			QFeedingThread::triggerGapUpdate(*it);
			std::cout << " >> Gap increased to " << QFeedingThread::currentGap << '.' << std::endl;
		}
		else
			std::cout << " >> Failed to increase gap!" << std::endl;
	}
	else if (QFeedingThread::numberOfBars + 5u < numberOfGrayBars)
	{
		if (QFeedingThread::Gaps[0u] != QFeedingThread::currentGap)
		{
			auto it = QFeedingThread::Gaps.cbegin();
			
			while (QFeedingThread::currentGap != *it)
				++it;
			
			--it;
			QFeedingThread::currentGap = *it;
			QFeedingThread::triggerGapUpdate(*it);
			std::cout << " >> Gap decreased to " << QFeedingThread::currentGap << '.' << std::endl;
		}
		else
			std::cout << " >> Failed to decrease gap!" << std::endl;
	}
}

quint32 QFeedingThread::updateTerminal(const HexScanReport& report, quint32 realTime, qreal volatility) const
{
	HexTerminalFile* info = new HexTerminalFile(realTime, QFeedingThread::currentGap, volatility);
	return QScalarThread::generalTerminalUpdate(report, info);
}

#endif 
