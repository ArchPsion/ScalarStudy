#ifndef __DATA_CENTER_HPP__
#define __DATA_CENTER_HPP__

// Qt Libraries
 #include <QDate>
 #include <QDir>
 #include <QFile>

// Standard Libraries
#include <algorithm>
#include <iostream>

// Personal Libraries
#include "HexDayHistory.hpp"

class HexDataCenter
{
	private:
	
		inline static void				Count(const std::vector<HexFullFile>&, qint32&, qint32&, qint32&);
		inline static void				WriteReport(const std::vector<HexFullFile>&, HexFullReport&, qreal, qreal, bool);
		
		std::vector<HexDayHistory>			candlestickData;
		std::vector<HexScanFile>			posiPrecedents;
		std::vector<HexScanFile>			negaPrecedents;
		std::vector<HexFullFile>			positivePrecedents;
		std::vector<HexFullFile>			negativePrecedents;
		
		QString						currentDirectory;
		qreal						bestScalar;
		
		inline void					clear(bool);
		inline void					fullStudyFile(HexFullFile&, qreal, qreal) const;
		inline void					quickStudyFile(HexScanFile&, qreal) const;
	
	public:
	
		inline std::vector<HexCandlestick<qreal>>	getCandlesticks(qint32, bool) const;
		inline const std::vector<HexFullFile>&		getNegativePrecedents(void) const;
		inline const std::vector<HexFullFile>&		getPositivePrecedents(void) const;
		inline HexFullReport				fullReport(qreal, qreal) const;
		inline void					fullStudy(qreal, qreal);
		inline void					importFile(const QString&);
		inline bool					importDirectory(const QString&, quint32);
		inline void					locateBest(std::vector<HexCandlestick<qreal>>::const_iterator, quint32, quint32, quint32, quint32, qreal, bool, bool);
		inline void					locateGraduates(std::vector<HexCandlestick<qreal>>::const_iterator, quint32, quint32, quint32, qreal, qreal, bool, bool);
		inline void					quickStudy(qreal);
		inline HexScanReport				scanReport(qreal) const;
		inline void					setUpHistory(quint32, quint32, quint32);
};

void HexDataCenter::clear(bool scan)
{
	if (scan)
	{
		HexDataCenter::posiPrecedents.clear();
		HexDataCenter::negaPrecedents.clear();
	}
	else
	{
		HexDataCenter::positivePrecedents.clear();
		HexDataCenter::negativePrecedents.clear();
	}
}

void HexDataCenter::Count(const std::vector<HexFullFile>& precedents, qint32& bWin, qint32& eWin, qint32& sWin)
{
	for (const auto& file : precedents)
	{
		switch (file.foo)
		{
			case 'b':
				++bWin;
				break;
			
			case 'e':
				++eWin;
				break;
			
			case 's':
				++sWin;
				break;
			
			default:
				break;
		}
	}
}

HexFullReport HexDataCenter::fullReport(qreal takeProfit, qreal stopLoss) const
{
	HexFullReport report;
	
	HexDataCenter::WriteReport(positivePrecedents, report, takeProfit, stopLoss, true);
	HexDataCenter::WriteReport(negativePrecedents, report, takeProfit, stopLoss, false);
	
	if (not HexDataCenter::positivePrecedents.empty())
		report.worstSimilarity = HexDataCenter::positivePrecedents.back().similarity;
	
	if ((not HexDataCenter::negativePrecedents.empty()) and HexDataCenter::negativePrecedents.back().similarity < report.worstSimilarity)
		report.worstSimilarity = HexDataCenter::negativePrecedents.back().similarity;
	
	return report;
}

void HexDataCenter::fullStudy(qreal tp, qreal sl)
{
	for (auto& file : HexDataCenter::positivePrecedents)
		HexDataCenter::fullStudyFile(file, file.factor*tp, file.factor*sl);
	
	for (auto& file : HexDataCenter::negativePrecedents)
		HexDataCenter::fullStudyFile(file, file.factor*tp, file.factor*sl);
}

void HexDataCenter::fullStudyFile(HexFullFile& file, qreal equivalentTP, qreal equivalentSL) const
{
	file.bestBuyProfit = HexDataCenter::candlestickData[file.day].bestBuyAndSell(file.tradeTimeSpot, file.lastHigh, equivalentSL);
	file.bestSellProfit = HexDataCenter::candlestickData[file.day].bestSellAndBuy(file.tradeTimeSpot, file.lastLow, equivalentSL);
	file.foo = (file.bestBuyProfit >= equivalentTP ? (file.bestSellProfit >= equivalentTP ? 'e' : 'b' ) : (file.bestSellProfit >= equivalentTP ? 's' : 'u' ));
}

std::vector<HexCandlestick<qreal>> HexDataCenter::getCandlesticks(qint32 count, bool positive) const
{
	const auto& sf = (positive ? HexDataCenter::positivePrecedents[static_cast<quint32>(count)] : HexDataCenter::negativePrecedents[static_cast<quint32>(count)]);
	const auto numberOfCandlesticks = (sf.tradeTimeSpot - sf.sampleTimeSpot)/sf.timeUnit;
	
	const auto sizeOfHistory = HexDataCenter::candlestickData[sf.day].sizeOfHistory();
	const auto numberOfCandlesticksToDraw = (sf.sampleTimeSpot + (numberOfCandlesticks*6u/5u)*sf.timeUnit < sizeOfHistory ? numberOfCandlesticks*6u/5u : numberOfCandlesticks);
	return HexDataCenter::candlestickData[sf.day].getCandlesticks(sf.sampleTimeSpot, sf.timeUnit, numberOfCandlesticksToDraw);
}

const std::vector<HexFullFile>& HexDataCenter::getNegativePrecedents(void) const
{
	return HexDataCenter::negativePrecedents;
}

const std::vector<HexFullFile>& HexDataCenter::getPositivePrecedents(void) const
{
	return HexDataCenter::positivePrecedents;
}

bool HexDataCenter::importDirectory(const QString& directory, quint32 nb)
{
	const auto path = "input/" + directory + '/';
	QDir dataDir(path);
	
	dataDir.setNameFilters({ "*.txt" });
	dataDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	dataDir.setSorting(QDir::Name);
	
	if (!dataDir.exists())
	{
		std::cout << "Could not find directory [" << path.toStdString() + "]." << std::endl;
		return false;
	}
	
	const auto files = dataDir.entryList();
	const auto numberOfFiles = files.size();
	
	if (nb > numberOfFiles)
	{
		std::cout << "Not enough files to import." << std::endl;
		return false;
	}
	
	if (HexDataCenter::currentDirectory != directory)
	{
		HexDataCenter::candlestickData.clear();
		HexDataCenter::currentDirectory = directory;
	}
	
	while (HexDataCenter::candlestickData.size() > nb)
		HexDataCenter::candlestickData.pop_back();
	
	if (HexDataCenter::candlestickData.size() == nb)
		return true;
	
	for (auto it = files.crbegin() + HexDataCenter::candlestickData.size(); it != files.crend(); ++it)
	{
		const auto filePath = path + (*it);
		HexDataCenter::importFile(filePath);
		
		if (HexDataCenter::candlestickData.size() == nb)
			break;
	}
	
	return (HexDataCenter::candlestickData.size() == nb);
}

void HexDataCenter::importFile(const QString& filePath)
{
	QFile dataFile(filePath);
	
	if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		std::cout << "Could not read file [" << filePath.toStdString() + "]." << std::endl;
		return;
	}
	
	QTextStream fileReader(&dataFile);
	const auto date = QDate::fromString(filePath.split('_')[1u], "yyyyMMdd").toString("ddd dd MMM");
	auto& record = HexDataCenter::candlestickData.emplace_back(date);
	
	fileReader.readLine();
	fileReader.readLine();
	
	while (!fileReader.atEnd())
	{
		const auto numbers = fileReader.readLine().split(' ');
		
		if (numbers.size() == 2u)
			record.addEntry(numbers[0u].toDouble(), numbers[1u].toDouble());
		else
			std::cout << "File [" << filePath.toStdString() + "] has unappropriate entry." << std::endl;
	}
}

void HexDataCenter::locateBest(std::vector<HexCandlestick<qreal>>::const_iterator it, quint32 timeUnit, quint32 feedingTimeUnit, quint32 numberOfCandlesticks, quint32 numberOfCandidates, qreal now, bool nemesis, bool feedOrScan)
{
	HexNormalisedInfo straightPattern(it, timeUnit/feedingTimeUnit, numberOfCandlesticks);
	HexNormalisedInfo reversedPattern(straightPattern);
	
	if (straightPattern.norm == 0.)
	{
		std::cout << "Norm equals 0." << std::endl;
		return;
	}
	
	feedOrScan ? HexDataCenter::posiPrecedents.clear() : positivePrecedents.clear();
	feedOrScan ? HexDataCenter::negaPrecedents.clear() : negativePrecedents.clear();
	
	const auto numberOfElementaryCandlesticks = timeUnit*numberOfCandlesticks;
	auto day = 0u;
	
	HexCandidatePool fullPool(HexDataCenter::positivePrecedents, HexDataCenter::negativePrecedents, numberOfElementaryCandlesticks, numberOfCandidates);
	HexCandidatePool scanPool(HexDataCenter::posiPrecedents, HexDataCenter::negaPrecedents, numberOfElementaryCandlesticks, numberOfCandidates);
	
	if (nemesis)
	{
		if (feedOrScan)
		{
			for (const auto& foo : candlestickData)
				foo.locateBestWithNemesis(straightPattern, reversedPattern, scanPool, day++, numberOfCandidates, now);
		}
		else
		{
			for (const auto& foo : candlestickData)
				foo.locateBestWithNemesis(straightPattern, reversedPattern, fullPool, day++, numberOfCandidates, now);
		}
	}
	else
	{
		if (feedOrScan)
		{
			for (const auto& foo : candlestickData)
				foo.locateBestWithoutNemesis(straightPattern, scanPool, day++, numberOfCandidates, now);
		}
		else
		{
			for (const auto& foo : candlestickData)
				foo.locateBestWithoutNemesis(straightPattern, fullPool, day++, numberOfCandidates, now);
		}
	}
	
	feedOrScan ? std::sort(HexDataCenter::posiPrecedents.begin(), HexDataCenter::posiPrecedents.end()) : std::sort(HexDataCenter::positivePrecedents.begin(), HexDataCenter::positivePrecedents.end());
	feedOrScan ? std::sort(HexDataCenter::negaPrecedents.begin(), HexDataCenter::negaPrecedents.end()) : std::sort(HexDataCenter::negativePrecedents.begin(), HexDataCenter::negativePrecedents.end());
}

void HexDataCenter::locateGraduates(std::vector<HexCandlestick<qreal>>::const_iterator it, quint32 timeUnit, quint32 feedingTimeUnit, quint32 numberOfCandlesticks, qreal threshold, qreal now, bool nemesis, bool feedOrScan)
{
	HexNormalisedInfo straightPattern(it, timeUnit/feedingTimeUnit, numberOfCandlesticks);
	HexNormalisedInfo reversedPattern(straightPattern);
	
	if (straightPattern.norm == 0.)
	{
		std::cout << "Norm equals 0." << std::endl;
		return;
	}
	
	feedOrScan ? HexDataCenter::posiPrecedents.clear() : positivePrecedents.clear();
	feedOrScan ? HexDataCenter::negaPrecedents.clear() : negativePrecedents.clear();
	
	const auto numberOfElementaryCandlesticks = timeUnit*numberOfCandlesticks;
	auto day = 0u;
	
	HexGraduatePool fullPool(HexDataCenter::positivePrecedents, HexDataCenter::negativePrecedents, numberOfElementaryCandlesticks);
	HexGraduatePool scanPool(HexDataCenter::posiPrecedents, HexDataCenter::negaPrecedents, numberOfElementaryCandlesticks);
	
	HexDataCenter::bestScalar = 0.;
	
	if (nemesis)
	{
		if (feedOrScan)
		{
			for (const auto& foo : candlestickData)
			{
				const auto best = foo.locateGraduatesWithNemesis(straightPattern, reversedPattern, scanPool, day++, threshold, now);
				HexDataCenter::bestScalar = std::max(best, HexDataCenter::bestScalar);
			}
		}
		else
		{
			for (const auto& foo : candlestickData)
			{
				const auto best = foo.locateGraduatesWithNemesis(straightPattern, reversedPattern, fullPool, day++, threshold, now);
				HexDataCenter::bestScalar = std::max(best, HexDataCenter::bestScalar);
			}
		}
	}
	else
	{
		if (feedOrScan)
		{
			for (const auto& foo : candlestickData)
			{
				const auto best = foo.locateGraduatesWithoutNemesis(straightPattern, scanPool, day++, threshold, now);
				HexDataCenter::bestScalar = std::max(best, HexDataCenter::bestScalar);
			}
		}
		else
		{
			for (const auto& foo : candlestickData)
			{
				const auto best = foo.locateGraduatesWithoutNemesis(straightPattern, fullPool, day++, threshold, now);
				HexDataCenter::bestScalar = std::max(best, HexDataCenter::bestScalar);
			}
		}
	}
	
	feedOrScan ? std::sort(HexDataCenter::posiPrecedents.begin(), HexDataCenter::posiPrecedents.end()) : std::sort(HexDataCenter::positivePrecedents.begin(), HexDataCenter::positivePrecedents.end());
	feedOrScan ? std::sort(HexDataCenter::negaPrecedents.begin(), HexDataCenter::negaPrecedents.end()) : std::sort(HexDataCenter::negativePrecedents.begin(), HexDataCenter::negativePrecedents.end());
}

void HexDataCenter::quickStudy(qreal sl)
{
	for (auto& file : HexDataCenter::posiPrecedents)
		HexDataCenter::quickStudyFile(file, file.factor*sl);
	
	for (auto& file : HexDataCenter::negaPrecedents)
		HexDataCenter::quickStudyFile(file, file.factor*sl);
}

void HexDataCenter::quickStudyFile(HexScanFile& file, qreal equivalentSL) const
{
	file.bestBuy = HexDataCenter::candlestickData[file.day].bestBuyAndSell(file.tradeTimeSpot, file.lastHigh, equivalentSL)/file.factor;
	file.bestSell = HexDataCenter::candlestickData[file.day].bestSellAndBuy(file.tradeTimeSpot, file.lastLow, equivalentSL)/file.factor;
}

HexScanReport HexDataCenter::scanReport(qreal tp) const
{
	auto report = HexScanReport(HexDataCenter::bestScalar, HexDataCenter::posiPrecedents.size() + HexDataCenter::negaPrecedents.size());
	
	for (const auto& sf : HexDataCenter::posiPrecedents)
	{
		report.bestBuys.push_back(sf.bestBuy);
		report.bestSells.push_back(sf.bestSell);
		
		if (sf.bestBuy >= tp)
		{
			if (sf.bestSell >= tp)
				++report.eWin;
			else
				++report.bWin;
		}
		else if (sf.bestSell >= tp)
			++report.sWin;
	}
	
	for (const auto& sf : HexDataCenter::negaPrecedents)
	{
		report.bestBuys.push_back(sf.bestSell);
		report.bestSells.push_back(sf.bestBuy);
		
		if (sf.bestBuy >= tp)
		{
			if (sf.bestSell >= tp)
				++report.eWin;
			else
				++report.sWin;
		}
		else if (sf.bestSell >= tp)
			++report.bWin;
	}
	
	if (not HexDataCenter::posiPrecedents.empty())
		report.worstSimilarity = HexDataCenter::posiPrecedents.back().similarity;
	
	if ((not HexDataCenter::negaPrecedents.empty()) and HexDataCenter::negaPrecedents.back().similarity < report.worstSimilarity)
		report.worstSimilarity = HexDataCenter::negaPrecedents.back().similarity;
	
	std::sort(report.bestBuys.begin(), report.bestBuys.end());
	std::sort(report.bestSells.begin(), report.bestSells.end());
	return report;
}

void HexDataCenter::setUpHistory(quint32 timeUnit, quint32 numberOfCandlesticks, quint32 scope)
{
	for (auto& foo : candlestickData)
		foo.setUpHistory(timeUnit, numberOfCandlesticks, scope);
}

void HexDataCenter::WriteReport(const std::vector<HexFullFile>& precedents, HexFullReport& report, qreal takeProfit, qreal stopLoss, bool positive)
{
	if (precedents.empty())
		return;
	
	auto bWin = 0;
	auto eWin = 0;
	auto sWin = 0;
	
	HexDataCenter::Count(precedents, bWin, eWin, sWin);
	
	const auto numberOfClones = precedents.size();
	const auto bGain = static_cast<qreal>(bWin + eWin)*takeProfit - static_cast<qreal>(sWin)*stopLoss;
	const auto sGain = static_cast<qreal>(sWin + eWin)*takeProfit - static_cast<qreal>(bWin)*stopLoss;
	
	report.numberOfSamples += numberOfClones;
	report.eWin += eWin;
	
	if (positive)
	{
		report.bSuccessRate = static_cast<qreal>(bWin + eWin)*100./static_cast<qreal>(numberOfClones);
		report.bAverageGain = bGain/static_cast<qreal>(numberOfClones);
		
		report.sSuccessRate = static_cast<qreal>(sWin + eWin)*100./static_cast<qreal>(numberOfClones);
		report.sAverageGain = sGain/static_cast<qreal>(numberOfClones);
		
		report.bWin += bWin;
		report.sWin += sWin;
	}
	else
	{
		report.bExpandedSuccessRate = static_cast<qreal>(bWin + eWin)*100./static_cast<qreal>(numberOfClones);
		report.bExpandedAverageGain = bGain/static_cast<qreal>(numberOfClones);
		
		report.sExpandedSuccessRate = static_cast<qreal>(sWin + eWin)*100./static_cast<qreal>(numberOfClones);
		report.sExpandedAverageGain = sGain/static_cast<qreal>(numberOfClones);
		
		report.bWin += sWin;
		report.sWin += bWin;
	}
}

#endif
