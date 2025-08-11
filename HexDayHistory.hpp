#ifndef __DAY_HISTORY_HPP__
#define __DAY_HISTORY_HPP__

// Personal Libraries
#include "HexCandidatePool.hpp"
#include "HexGraduatePool.hpp"

class HexDayHistory
{
	private:
	
		const QString									date;
		std::vector<HexCandlestick<qreal>>						history;
		std::vector<HexNormalisedInfo>							normalisedHistory;
		
		quint32										numberOfCandlesticks = 0u;
		quint32										timeUnit = 0u;
		quint32										scope = 0u;
		
		inline QString									timeString(quint32) const;
	
	public:
		
		inline										HexDayHistory(const QString& str);
		inline void									addEntry(qreal, qreal);
		inline qreal									bestBuyAndSell(quint32, qreal, qreal) const;
		inline qreal									bestSellAndBuy(quint32, qreal, qreal) const;
		inline std::vector<HexCandlestick<qreal>>						getCandlesticks(quint32, quint32, quint32) const;
		template<class FileClass> inline void						locateBestWithNemesis(const HexNormalisedInfo&, const HexNormalisedInfo&, HexCandidatePool<FileClass>&, quint32, quint32, qreal) const;
		template<class FileClass> inline void						locateBestWithoutNemesis(const HexNormalisedInfo&, HexCandidatePool<FileClass>&, quint32, quint32, qreal) const;
		template<class FileClass> inline qreal						locateGraduatesWithNemesis(const HexNormalisedInfo&, const HexNormalisedInfo&, HexGraduatePool<FileClass>&, quint32, qreal, qreal) const;
		template<class FileClass> inline qreal						locateGraduatesWithoutNemesis(const HexNormalisedInfo&, HexGraduatePool<FileClass>&, quint32, qreal, qreal) const;
		inline void									setUpHistory(quint32, quint32, quint32);
		inline quint32									sizeOfHistory(void) const;
		inline quint32									strictBuyAndSell(quint32, qreal) const;
		inline quint32									strictSellAndBuy(quint32, qreal) const;
};

HexDayHistory::HexDayHistory(const QString& str) : date(str)
{
	HexDayHistory::history.reserve(23'400u);
}

void HexDayHistory::addEntry(qreal low, qreal high)
{
	HexDayHistory::history.emplace_back(low, high);
}

qreal HexDayHistory::bestBuyAndSell(quint32 tradeTimeSpot, qreal buyPrice, qreal stopLoss) const
{
	const auto end = HexDayHistory::history.cend();
	auto it = HexDayHistory::history.cbegin() + tradeTimeSpot;
	
	const auto pricelimit = buyPrice - stopLoss;
	auto max = buyPrice;
	
	while (it != end and pricelimit < it->low)
	{
		if (it->high > max)
			max = it->high;
		
		++it;
	}
	
	return (max - buyPrice);
}

qreal HexDayHistory::bestSellAndBuy(quint32 tradeTimeSpot, qreal sellPrice, qreal stopLoss) const
{
	const auto end = HexDayHistory::history.cend();
	auto it = HexDayHistory::history.cbegin() + tradeTimeSpot;
	
	const auto pricelimit = sellPrice + stopLoss;
	auto min = sellPrice;
	
	while (it != end and it->high < pricelimit)
	{
		if (it->low < min)
			min = it->low;
		
		++it;
	}
	
	return (sellPrice - min);
}

std::vector<HexCandlestick<qreal>> HexDayHistory::getCandlesticks(quint32 start, quint32 timeUnit, quint32 numberOfCandlesticks) const
{
	auto it = HexDayHistory::history.cbegin() + start;
	std::vector<HexCandlestick<qreal>> foo;
	foo.reserve(numberOfCandlesticks);
	
	for (auto i = 0u; i < numberOfCandlesticks; ++i)
	{
		auto& c = foo.emplace_back(it->low, it->high);
		++it;
		
		for (auto j = 1u; j < timeUnit; ++j)
		{
			if (it->low < c.low)
				c.low = it->low;
			
			if (it->high > c.high)
				c.high = it->high;
			
			++it;
		}
	}
	
	return foo;
}

template<class FileClass>
void HexDayHistory::locateBestWithNemesis(const HexNormalisedInfo& straightPattern, const HexNormalisedInfo& reversedPattern, HexCandidatePool<FileClass>& pool, quint32 day, quint32 numberOfCandidates, qreal now) const
{
	const auto numberOfElementaryCandlesticks = HexDayHistory::timeUnit*HexDayHistory::numberOfCandlesticks;
	const auto realTimeSpot = static_cast<quint32>(now*static_cast<qreal>(HexDayHistory::history.size()));
	
	const auto iFile = HexIterationFile(HexDayHistory::normalisedHistory, (now < 0. or now > 1. ? 50'000u : realTimeSpot), HexDayHistory::scope, numberOfElementaryCandlesticks);
	auto tradeTimeSpot = iFile.sampleTimeSpot + numberOfElementaryCandlesticks;
	
	for (auto it = HexDayHistory::normalisedHistory.cbegin() + iFile.sampleTimeSpot; it != iFile.end; ++it)
	{
		const auto scalar1 = straightPattern.scalar(*it);
		const auto scalar2 = reversedPattern.scalar(*it);
		
		if (scalar1 >= scalar2)
		{
			if (pool.numberOfResults() < numberOfCandidates or pool.worstSimilarity() < scalar1)
			{
				if constexpr(std::is_same<FileClass, HexFullFile>::value)
				{
					HexFullFile file(HexDayHistory::date, HexDayHistory::timeString(tradeTimeSpot), day, tradeTimeSpot - numberOfElementaryCandlesticks, tradeTimeSpot, timeUnit, scalar1, it->norm/straightPattern.norm, it->lastLow, it->lastHigh);
					pool.addPositiveCandidate(file);
				}
				else
				{
					HexScanFile file(day, tradeTimeSpot, scalar1, it->norm/straightPattern.norm, it->lastLow, it->lastHigh);
					pool.addPositiveCandidate(file);
				}
			}
		}
		else if (pool.numberOfResults() < numberOfCandidates or pool.worstSimilarity() < scalar2)
		{
			if constexpr(std::is_same<FileClass, HexFullFile>::value)
			{
				HexFullFile file(HexDayHistory::date, HexDayHistory::timeString(tradeTimeSpot), day, tradeTimeSpot - numberOfElementaryCandlesticks, tradeTimeSpot, HexDayHistory::timeUnit, scalar2, it->norm/reversedPattern.norm, it->lastLow, it->lastHigh);
				pool.addNegativeCandidate(file);
			}
			else
			{
				HexScanFile file(day, tradeTimeSpot, scalar2, it->norm/reversedPattern.norm, it->lastLow, it->lastHigh);
				pool.addNegativeCandidate(file);
			}
		}
		
		++tradeTimeSpot;
	}
}

template<class FileClass>
void HexDayHistory::locateBestWithoutNemesis(const HexNormalisedInfo& straightPattern, HexCandidatePool<FileClass>& pool, quint32 day, quint32 numberOfCandidates, qreal now) const
{
	const auto numberOfElementaryCandlesticks = HexDayHistory::timeUnit*HexDayHistory::numberOfCandlesticks;
	const auto realTimeSpot = static_cast<quint32>(now*static_cast<qreal>(HexDayHistory::history.size()));
	
	const auto iFile = HexIterationFile(HexDayHistory::normalisedHistory, (now < 0. or now > 1. ? 50'000u : realTimeSpot), HexDayHistory::scope, numberOfElementaryCandlesticks);
	auto tradeTimeSpot = iFile.sampleTimeSpot + numberOfElementaryCandlesticks;
	
	for (auto it = HexDayHistory::normalisedHistory.cbegin() + iFile.sampleTimeSpot; it != iFile.end; ++it)
	{
		const auto scalar = straightPattern.scalar(*it);
		
		if (pool.numberOfResults() < numberOfCandidates or pool.worstSimilarity() < scalar)
		{
			if constexpr(std::is_same<FileClass, HexFullFile>::value)
			{
				HexFullFile file(HexDayHistory::date, HexDayHistory::timeString(tradeTimeSpot), day, tradeTimeSpot - numberOfElementaryCandlesticks, tradeTimeSpot, HexDayHistory::timeUnit, scalar, it->norm/straightPattern.norm, it->lastLow, it->lastHigh);
				pool.addPositiveCandidate(file);
			}
			else
			{
				HexScanFile file(day, tradeTimeSpot, scalar, it->norm/straightPattern.norm, it->lastLow, it->lastHigh);
				pool.addPositiveCandidate(file);
			}
		}
		
		++tradeTimeSpot;
	}
}

template<class FileClass>
qreal HexDayHistory::locateGraduatesWithNemesis(const HexNormalisedInfo& straightPattern, const HexNormalisedInfo& reversedPattern, HexGraduatePool<FileClass>& pool, quint32 day, qreal threshold, qreal now) const
{
	const auto numberOfElementaryCandlesticks = HexDayHistory::timeUnit*HexDayHistory::numberOfCandlesticks;
	const auto realTimeSpot = static_cast<quint32>(now*static_cast<qreal>(HexDayHistory::history.size()));
	
	const auto iFile = HexIterationFile(HexDayHistory::normalisedHistory, (now < 0. or now > 1. ? 50'000u : realTimeSpot), HexDayHistory::scope, numberOfElementaryCandlesticks);
	auto tradeTimeSpot = iFile.sampleTimeSpot + numberOfElementaryCandlesticks;
	auto best = 0.;
	
	for (auto it = HexDayHistory::normalisedHistory.cbegin() + iFile.sampleTimeSpot; it != iFile.end; ++it)
	{
		const auto scalar1 = straightPattern.scalar(*it);
		const auto scalar2 = reversedPattern.scalar(*it);
		best = std::max(std::max(scalar1, scalar2), best);
		
		if (scalar1 >= scalar2)
		{
			if (scalar1 >= threshold)
			{
				if constexpr(std::is_same<FileClass, HexFullFile>::value)
				{
					HexFullFile file(HexDayHistory::date, HexDayHistory::timeString(tradeTimeSpot), day, tradeTimeSpot - numberOfElementaryCandlesticks, tradeTimeSpot, HexDayHistory::timeUnit, scalar1, it->norm/straightPattern.norm, it->lastLow, it->lastHigh);
					pool.addPositiveGraduate(file);
				}
				else
				{
					HexScanFile file(day, tradeTimeSpot, scalar1, it->norm/straightPattern.norm, it->lastLow, it->lastHigh);
					pool.addPositiveGraduate(file);
				}
			}
		}
		else if (scalar2 >= threshold)
		{
			if constexpr(std::is_same<FileClass, HexFullFile>::value)
			{
				HexFullFile file(HexDayHistory::date, HexDayHistory::timeString(tradeTimeSpot), day, tradeTimeSpot - numberOfElementaryCandlesticks, tradeTimeSpot, HexDayHistory::timeUnit, scalar2, it->norm/reversedPattern.norm, it->lastLow, it->lastHigh);
				pool.addNegativeGraduate(file);
			}
			else
			{
				HexScanFile file(day, tradeTimeSpot, scalar2, it->norm/reversedPattern.norm, it->lastLow, it->lastHigh);
				pool.addNegativeGraduate(file);
			}
		}
		
		++tradeTimeSpot;
	}
	
	return best;
}

template<class FileClass>
qreal HexDayHistory::locateGraduatesWithoutNemesis(const HexNormalisedInfo& straightPattern, HexGraduatePool<FileClass>& pool, quint32 day, qreal threshold, qreal now) const
{
	const auto numberOfElementaryCandlesticks = HexDayHistory::timeUnit*HexDayHistory::numberOfCandlesticks;
	const auto realTimeSpot = static_cast<quint32>(now*static_cast<qreal>(HexDayHistory::history.size()));
	
	const auto iFile = HexIterationFile(HexDayHistory::normalisedHistory, (now < 0. or now > 1. ? 50'000u : realTimeSpot), HexDayHistory::scope, numberOfElementaryCandlesticks);
	auto tradeTimeSpot = iFile.sampleTimeSpot + numberOfElementaryCandlesticks;
	auto best = 0.;
	
	for (auto it = HexDayHistory::normalisedHistory.cbegin() + iFile.sampleTimeSpot; it != iFile.end; ++it)
	{
		const auto scalar = straightPattern.scalar(*it);
		best = std::max(scalar, best);
		
		if (scalar >= threshold)
		{
			if constexpr(std::is_same<FileClass, HexFullFile>::value)
			{
				HexFullFile file(HexDayHistory::date, HexDayHistory::timeString(tradeTimeSpot), day, tradeTimeSpot - numberOfElementaryCandlesticks, tradeTimeSpot, HexDayHistory::timeUnit, scalar, it->norm/straightPattern.norm, it->lastLow, it->lastHigh);
				pool.addPositiveGraduate(file);
			}
			else
			{
				HexScanFile file(day, tradeTimeSpot, scalar, it->norm/straightPattern.norm, it->lastLow, it->lastHigh);
				pool.addPositiveGraduate(file);
			}
		}
		
		++tradeTimeSpot;
	}
	
	return best;
}

void HexDayHistory::setUpHistory(quint32 tu, quint32 nc, quint32 sc)
{
	HexDayHistory::scope = sc;
	
	if (HexDayHistory::timeUnit == tu and HexDayHistory::numberOfCandlesticks == nc)
		return;
	
	const auto numberOfElementaryCandlesticks = nc*tu;
	HexDayHistory::numberOfCandlesticks = nc;
	HexDayHistory::timeUnit = tu;
	
	if (HexDayHistory::history.size() < numberOfElementaryCandlesticks)
		return;
	
	const auto size = HexDayHistory::history.size();
	const auto timeSpotLimit = size - numberOfElementaryCandlesticks;
	auto it = HexDayHistory::history.cbegin();
	
	HexDayHistory::normalisedHistory.clear();
	HexDayHistory::normalisedHistory.reserve(timeSpotLimit);
	
	for (auto sampleTimeSpot = 0u; sampleTimeSpot < timeSpotLimit; ++sampleTimeSpot)
	{
		HexDayHistory::normalisedHistory.emplace_back(it, timeUnit, numberOfCandlesticks);
		++it;
	}
}

quint32 HexDayHistory::sizeOfHistory(void) const
{
	return HexDayHistory::history.size();
}

quint32 HexDayHistory::strictBuyAndSell(quint32 tradeTimeSpot, qreal goal) const
{
	const auto end = HexDayHistory::history.cend();
	auto it = HexDayHistory::history.cbegin() + tradeTimeSpot;
	auto count = 0u;
	
	while (it != end and it->high < goal)
	{
		++it;
		++count;
	}
	
	return (it != end and it->high >= goal ? count : 50'000u);
}

quint32 HexDayHistory::strictSellAndBuy(quint32 tradeTimeSpot, qreal goal) const
{
	const auto end = HexDayHistory::history.cend();
	auto it = HexDayHistory::history.cbegin() + tradeTimeSpot;
	auto count = 0u;
	
	while (it != end and goal < it->low)
	{
		++it;
		++count;
	}
	
	return (it != end and it->low <= goal ? count : 50'000u);
}

QString HexDayHistory::timeString(quint32 timeSpot) const
{
	const auto timestamp = timeSpot*23'400u/HexDayHistory::history.size();
	const auto hour = 15u + (timestamp + 1'800u)/3'600u;
	const auto minute = (timestamp/60u + 30u) % 60u;
	const auto second = timestamp % 60u;
	
	const QString zeroPadding1 = (minute < 10u ? "0" : "");
	const QString zeroPadding2 = (second < 10u ? "0" : "");
	return QString::number(hour) + ':' + zeroPadding1 + QString::number(minute) + ':' + zeroPadding2 + QString::number(second);
}

#endif
