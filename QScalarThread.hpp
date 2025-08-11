#ifndef __SCALAR_THREAD_HPP__
#define __SCALAR_THREAD_HPP__

// Standard Libraries
#include <string>
using std::string_literals::operator""s;

// Qt Libraries
#include <QThread>

// Personal Libraries
#include "HexDataCenter.hpp"

class QScalarThread : public QThread
{
	Q_OBJECT
	
	private:
		
		inline static QString						Number(qreal);
	
	protected:
		
		static constexpr quint32					VolatilityCandlesticks = 10u;
		
		inline static qreal						AssessVolatility(std::vector<HexCandlestick<qreal>>::const_iterator, quint32);
		
		std::vector<HexCandlestick<qreal>>				candlesticksToBeAnalysed;
		HexDataCenter&							dataCollection;
		QString								directory;
		
		quint32								numberOfDays;
		quint32								scope;
		
		quint32								timeUnit;
		quint32								numberOfCandlesticks;
		
		quint32								numberOfCandidates;
		qreal								threshold;
		qreal								tradeBalance;
		
		qreal								takeProfit;
		qreal								stopLoss;
		
		bool								bestOverGraduates;
		bool								nemesis;
		bool								shouldContinue;
		
		inline void							generalSetUp(const HexCheckFile&, bool, bool);
		inline quint32							generalTerminalUpdate(const HexScanReport&, HexTerminalFile*) const;
	
	public:
	
		inline								QScalarThread(HexDataCenter&);
		inline void							stop(void);
	
	signals:
	
		void								triggerTerminalUpdate(HexTerminalFile*);
};

QScalarThread::QScalarThread(HexDataCenter& foo) : dataCollection(foo)
{
	QScalarThread::candlesticksToBeAnalysed.reserve(400u);
}

qreal QScalarThread::AssessVolatility(std::vector<HexCandlestick<qreal>>::const_iterator it, quint32 numberOfCandlesticks)
{
	auto itt = it + 1u;
	auto size = it->high - it->low;
	auto grow = 0.;
	
	for (auto i = 1u; i < numberOfCandlesticks; ++i)
	{
		const auto up = itt->high - it->high;
		const auto down = it->low - itt->low;
		
		if (up > 0.)
			grow += up;
		
		if (down > 0.)
			grow += down;
		
		size += (itt->high - itt->low);
		++it;
		++itt;
	}
	
	return size/static_cast<qreal>(numberOfCandlesticks) + grow/static_cast<qreal>(numberOfCandlesticks - 1u);
}

void QScalarThread::generalSetUp(const HexCheckFile& report, bool best, bool nms)
{
	QScalarThread::directory = report.directory;
	QScalarThread::numberOfDays = report.numberOfDays;
	QScalarThread::scope = report.scope;
	
	QScalarThread::timeUnit = report.timeUnit;
	QScalarThread::numberOfCandlesticks = report.numberOfCandlesticks;
	
	QScalarThread::numberOfCandidates = report.numberOfCandidates;
	QScalarThread::threshold = report.threshold;
	QScalarThread::tradeBalance = 1.000'001L - report.tradeBalance;
	
	QScalarThread::takeProfit = report.takeProfit;
	QScalarThread::stopLoss = report.stopLoss;
	
	QScalarThread::bestOverGraduates = best;
	QScalarThread::nemesis = nms;
	QScalarThread::shouldContinue = true;
}

quint32 QScalarThread::generalTerminalUpdate(const HexScanReport& report, HexTerminalFile* info) const
{
	info->worstSimilarity = static_cast<qreal>(report.worstSimilarity*100.);
	info->numberOfSamples = report.numberOfSamples;
	
	if (report.numberOfSamples >= 10u)
	{
		const auto b70 = report.bestBuys[report.numberOfSamples*3u/10u];
		const auto s70 = report.bestSells[report.numberOfSamples*3u/10u];
		
		info->buy70 = static_cast<qreal>(b70);
		info->buy80 = static_cast<qreal>(report.bestBuys[report.numberOfSamples/5u]);
		info->buy90 = static_cast<qreal>(report.bestBuys[report.numberOfSamples/10u]);
		
		info->sell70 = static_cast<qreal>(s70);
		info->sell80 = static_cast<qreal>(report.bestSells[report.numberOfSamples/5u]);
		info->sell90 = static_cast<qreal>(report.bestSells[report.numberOfSamples/10u]);
		
		if (report.bWin > report.sWin)
			info->ratio = 1'000u + (report.bWin + report.eWin)*100u/report.numberOfSamples;
		else
			info->ratio = (report.bWin < report.sWin ? 3'000u : 2'000u) + (report.sWin + report.eWin)*100u/report.numberOfSamples;
		
		if (report.worstSimilarity >= QScalarThread::threshold and report.numberOfSamples >= QScalarThread::numberOfCandidates)
		{
			const auto index = static_cast<quint32>(QScalarThread::tradeBalance*static_cast<qreal>(report.numberOfSamples));
			
			if (report.bestBuys[index] >= QScalarThread::takeProfit or report.bestSells[index] >= QScalarThread::takeProfit)
			{
				const_cast<QScalarThread*>(this)->triggerTerminalUpdate(info);
				return 2u;
			}
			
			if (b70 >= QScalarThread::takeProfit or s70 >= QScalarThread::takeProfit)
			{
				const_cast<QScalarThread*>(this)->triggerTerminalUpdate(info);
				return 1u;
			}
		}
	}
	else if (report.numberOfSamples != 0u)
	{
		auto count = 0u;
		
		for (const auto& b : report.bestBuys)
		{
			info->buy += (b != 0. ? QScalarThread::Number(b).left(4u) : QString("xxxx")) + "  ";
			++count;
			
			if (count == 8u)
				break;
		}
		
		info->buy.chop(2u);
		count = 0u;
		
		for (const auto& s : report.bestSells)
		{
			info->sell += (s != 0. ? QScalarThread::Number(s).left(4u) : QString("xxxx")) + "  ";
			++count;
			
			if (count == 8u)
				break;
		}
		
		info->sell.chop(2u);
	}
	
	const_cast<QScalarThread*>(this)->triggerTerminalUpdate(info);
	return 0u;
}

QString QScalarThread::Number(qreal nb)
{
	return QString::number(static_cast<double>(nb), 'f', 6);
}

void QScalarThread::stop(void)
{
	QScalarThread::shouldContinue = false;
}

#endif 
