#ifndef __ANALYSING_THREAD_HPP__
#define __ANALYSING_THREAD_HPP__

// Personal Libraries
#include "QScalarThread.hpp"

class QAnalysingThread : public QScalarThread
{
	Q_OBJECT
	
	private:
		
		quint32								startingSpot;
		
		inline void							runBest(void);
		inline void							runGraduates(void);
		inline void							updateTerminal(const HexScanReport&, quint32, qreal) const;
	
	protected:
	
		inline void							run(void) override;
	
	public:
	
		inline								QAnalysingThread(HexDataCenter&);
		inline void							setUp(const HexCheckFile&, bool, bool, const std::vector<HexCandlestick<qreal>>&);
};

QAnalysingThread::QAnalysingThread(HexDataCenter& foo) : QScalarThread(foo)
{
}

void QAnalysingThread::run(void)
{
	QScalarThread::bestOverGraduates ? QAnalysingThread::runBest() : QAnalysingThread::runGraduates();
	std::cout << "Scan " << (QScalarThread::shouldContinue ? "finished."s : "stopped."s) << std::endl;
}

void QAnalysingThread::runBest(void)
{
	const auto invalid = not QScalarThread::dataCollection.importDirectory(QScalarThread::directory, QScalarThread::numberOfDays);
	
	if (invalid)
		return;
	
	QScalarThread::dataCollection.setUpHistory(QScalarThread::timeUnit, QScalarThread::numberOfCandlesticks, QScalarThread::scope);
	
	const auto size = QScalarThread::candlesticksToBeAnalysed.size();
	const auto numberOfElementaryCandlesticks = QScalarThread::timeUnit*QScalarThread::numberOfCandlesticks;
	const auto timeLimit = size - numberOfElementaryCandlesticks;
	
	auto sampleTimeSpot = QAnalysingThread::startingSpot;
	auto tradeTimeSpot = sampleTimeSpot + numberOfElementaryCandlesticks;
	auto it = QScalarThread::candlesticksToBeAnalysed.cbegin() + QAnalysingThread::startingSpot;
	
	while (QScalarThread::shouldContinue and sampleTimeSpot < timeLimit)
	{
		const auto now = static_cast<qreal>(tradeTimeSpot)/static_cast<qreal>(size);
		const auto volatility = QScalarThread::AssessVolatility(it + (numberOfElementaryCandlesticks - QScalarThread::VolatilityCandlesticks), QScalarThread::VolatilityCandlesticks);
		
		QScalarThread::dataCollection.locateBest(it, QScalarThread::timeUnit, 1u, QScalarThread::numberOfCandlesticks, QScalarThread::numberOfCandidates, now, QScalarThread::nemesis, true);
		QScalarThread::dataCollection.quickStudy(QScalarThread::stopLoss);
		
		const auto report = QScalarThread::dataCollection.scanReport(QScalarThread::takeProfit);
		QAnalysingThread::updateTerminal(report, tradeTimeSpot, volatility);
		
		++sampleTimeSpot;
		++tradeTimeSpot;
		++it;
	}
}

void QAnalysingThread::runGraduates(void)
{
	const auto invalid = not QScalarThread::dataCollection.importDirectory(QScalarThread::directory, QScalarThread::numberOfDays);
	
	if (invalid)
		return;
	
	QScalarThread::dataCollection.setUpHistory(QScalarThread::timeUnit, QScalarThread::numberOfCandlesticks, QScalarThread::scope);
	
	const auto size = QScalarThread::candlesticksToBeAnalysed.size();
	const auto numberOfElementaryCandlesticks = QScalarThread::timeUnit*QScalarThread::numberOfCandlesticks;
	const auto timeLimit = size - numberOfElementaryCandlesticks;
	
	auto sampleTimeSpot = QAnalysingThread::startingSpot;
	auto tradeTimeSpot = sampleTimeSpot + numberOfElementaryCandlesticks;
	auto it = QScalarThread::candlesticksToBeAnalysed.cbegin() + QAnalysingThread::startingSpot;
	
	while (QScalarThread::shouldContinue and sampleTimeSpot < timeLimit)
	{
		const auto now = static_cast<qreal>(tradeTimeSpot)/static_cast<qreal>(size);
		const auto volatility = QScalarThread::AssessVolatility(it + (numberOfElementaryCandlesticks - QScalarThread::VolatilityCandlesticks), QScalarThread::VolatilityCandlesticks);
		
		QScalarThread::dataCollection.locateGraduates(it, QScalarThread::timeUnit, 1u, QScalarThread::numberOfCandlesticks, QScalarThread::threshold, now, QScalarThread::nemesis, true);
		QScalarThread::dataCollection.quickStudy(QScalarThread::stopLoss);
		
		const auto report = QScalarThread::dataCollection.scanReport(QScalarThread::takeProfit);
		QAnalysingThread::updateTerminal(report, tradeTimeSpot, volatility);
		
		++sampleTimeSpot;
		++tradeTimeSpot;
		++it;
	}
}
						
void QAnalysingThread::setUp(const HexCheckFile& report, bool best, bool nms, const std::vector<HexCandlestick<qreal>>& candlesticks)
{
	QScalarThread::generalSetUp(report, best, nms);
	QScalarThread::candlesticksToBeAnalysed = candlesticks;
	QAnalysingThread::startingSpot = report.tradeTimeSpot;
}

void QAnalysingThread::updateTerminal(const HexScanReport& report, quint32 tradeTimeSpot, qreal volatility) const
{
	HexTerminalFile* info = new HexTerminalFile(tradeTimeSpot*23'400u/QScalarThread::candlesticksToBeAnalysed.size(), tradeTimeSpot, volatility);
	QScalarThread::generalTerminalUpdate(report, info);
}

#endif 
