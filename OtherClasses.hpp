#ifndef __OTHER_CLASSES_HPP__
#define __OTHER_CLASSES_HPP__

// Standard Libraries
#include <cmath>
#include <vector>

//Qt Libraries
#include <QString>

enum class MissionEnum { Analyse, Feed, Locate, Scan };

template<typename Type>
class HexCandlestick
{
	public:
	
		Type low;
		Type high;
		
		inline HexCandlestick(Type l = static_cast<Type>(0), Type h = static_cast<Type>(0)) : low(l), high(h)
		{
		}
};

class HexCheckFile
{
	public:
	
		QString		directory;
		quint32		numberOfDays;
		quint32		scope;
		
		quint32		tradeTimeSpot;
		quint32		timeUnit;
		quint32		numberOfCandlesticks;
		qreal		firstCoefficient;
		
		quint32		numberOfCandidates;
		qreal		threshold;
		qreal		tradeBalance;
		
		qreal		takeProfit;
		qreal		stopLoss;
		
		quint32		feedingTimeUnit = 1u;
		bool		abort = true;
};

class HexFullFile
{
	public:
	
		QString date;
		QString time;
		
		quint32 day;
		quint32 sampleTimeSpot;
		quint32 tradeTimeSpot;
		quint32 timeUnit;
		
		qreal similarity;
		qreal factor;
		
		qreal lastLow;
		qreal lastHigh;
		
		qreal bestBuyProfit;
		qreal bestSellProfit;
		char foo;
		
		HexFullFile(const QString& dateString, const QString& timeString, quint32 d, quint32 sts, quint32 tts, quint32 tu, qreal s, qreal f, qreal l, qreal h) :
			date(dateString),
			time(timeString),
			day(d),
			sampleTimeSpot(sts),
			tradeTimeSpot(tts),
			timeUnit(tu),
			similarity(s),
			factor(f),
			lastLow(l),
			lastHigh(h),
			bestBuyProfit(0.),
			bestSellProfit(0.),
			foo('.')
		{
		}
		
		bool operator<(const HexFullFile& other) const
		{
			return (HexFullFile::similarity > other.similarity);
		}
		
		quint32 timeDistance(const HexFullFile& other) const
		{
			if (other.day != HexFullFile::day)
				return 50'000u;
			
			if (other.tradeTimeSpot >= HexFullFile::tradeTimeSpot)
				return other.tradeTimeSpot - HexFullFile::tradeTimeSpot;
			
			return HexFullFile::tradeTimeSpot - other.tradeTimeSpot;
		}
};

class HexFullReport
{
	public:
	
		qreal worstSimilarity = 2.;
		quint32 numberOfSamples = 0u;
		
		qint32 bWin = 0;
		qint32 eWin = 0;
		qint32 sWin = 0;
	
		qreal bSuccessRate = 0.;
		qreal bAverageGain = 0.;
		
		qreal sSuccessRate = 0.;
		qreal sAverageGain = 0.;
		
		qreal bExpandedSuccessRate = 0.;
		qreal bExpandedAverageGain = 0.;
		
		qreal sExpandedSuccessRate = 0.;
		qreal sExpandedAverageGain = 0.;
};

class HexNormalisedInfo
{
	private:
		
		static std::vector<qreal>			Coefficients;
	
	public:
		
		qreal					lastHigh;
		qreal					lastLow;
		qreal					norm;
		std::vector<HexCandlestick<qreal>>	normalisedCandlesticks;
		
		inline static void UpdateCoefficients(qreal firstCoefficient, quint32 numberOfCandlesticks)
		{
			HexNormalisedInfo::Coefficients.clear();
			HexNormalisedInfo::Coefficients.reserve(numberOfCandlesticks);
			
			const auto increment = (1. - firstCoefficient)/static_cast<qreal>(numberOfCandlesticks - 1u);
			
			for (auto i = 0u; i < numberOfCandlesticks; ++i)
				HexNormalisedInfo::Coefficients.push_back(firstCoefficient + static_cast<qreal>(i)*increment);
		}
		
		template<typename Type>
		HexNormalisedInfo(Type it, quint32 timeUnit, quint32 numberOfCandlesticks)
		{
			HexNormalisedInfo::normalisedCandlesticks.reserve(numberOfCandlesticks);
			auto mean = 0.;
			
			for (auto i = 0u; i < numberOfCandlesticks; ++i)
			{
				auto& cs = HexNormalisedInfo::normalisedCandlesticks.emplace_back(it->low, it->high);
				++it;
				
				for (auto j = 1u; j < timeUnit; ++j)
				{
					if (it->low < cs.low)
						cs.low = it->low;
					
					if (it->high > cs.high)
						cs.high = it->high;
					
					++it;
				}
				
				mean += cs.low + cs.high;
			}
			
			HexNormalisedInfo::lastLow = HexNormalisedInfo::normalisedCandlesticks.back().low;
			HexNormalisedInfo::lastHigh = HexNormalisedInfo::normalisedCandlesticks.back().high;
			
			mean /= static_cast<qreal>(numberOfCandlesticks*2.);
			
			auto itc = HexNormalisedInfo::Coefficients.cbegin();
			auto scalar = 0.;
			
			for (auto& cs : HexNormalisedInfo::normalisedCandlesticks)
			{
				cs.low -= mean;
				cs.high -= mean;
				
				scalar += (cs.low*cs.low + cs.high*cs.high)*(*itc);
				++itc;
			}
			
			HexNormalisedInfo::norm = sqrtl(scalar);
			
			for (auto& cs : HexNormalisedInfo::normalisedCandlesticks)
			{
				cs.low /= HexNormalisedInfo::norm;
				cs.high /= HexNormalisedInfo::norm;
			}
		}
		
		HexNormalisedInfo(const HexNormalisedInfo& other) : norm(other.norm)
		{
			HexNormalisedInfo::normalisedCandlesticks.reserve(other.normalisedCandlesticks.size());
			
			for (const auto& cs : other.normalisedCandlesticks)
				HexNormalisedInfo::normalisedCandlesticks.emplace_back(-cs.high, -cs.low);
		}
		
		qreal scalar(const HexNormalisedInfo& other) const
		{
			auto it = other.normalisedCandlesticks.cbegin();
			auto foo = 0.;
			
			if (HexNormalisedInfo::Coefficients[0u] != 1.)
			{
				auto itc = HexNormalisedInfo::Coefficients.cbegin();
				
				for (const auto& cs : HexNormalisedInfo::normalisedCandlesticks)
				{
					foo += (cs.low*it->low + cs.high*it->high)*(*itc);
					++it;
					++itc;
				}
			}
			else
			{
				for (const auto& cs : HexNormalisedInfo::normalisedCandlesticks)
				{
					foo += (cs.low*it->low + cs.high*it->high);
					++it;
				}
			}
			
			return foo;
		}
};

class HexIterationFile
{
	public:
	
		std::vector<HexNormalisedInfo>::const_iterator end;
		quint32 sampleTimeSpot;
		
		HexIterationFile(const std::vector<HexNormalisedInfo>& normalisedHistory, quint32 realTimeSpot, quint32 scope, quint32 numberOfElementaryCandlesticks)
		{
			if (realTimeSpot > 25'000u)
			{
				HexIterationFile::sampleTimeSpot = 0u;
				HexIterationFile::end = normalisedHistory.cend();
			}
			else
			{
				const auto finalSpot = (realTimeSpot > numberOfElementaryCandlesticks ? realTimeSpot - numberOfElementaryCandlesticks : 0u);
				const auto stop = finalSpot + scope;
				
				HexIterationFile::sampleTimeSpot = (finalSpot > scope ? finalSpot - scope : 0u);
				HexIterationFile::end = (stop >= normalisedHistory.size() ? normalisedHistory.cend() : normalisedHistory.cbegin() + stop);
			}
		}
};

class HexScanFile
{
	public:
	
		quint32 day;
		quint32 tradeTimeSpot;
		
		qreal similarity;
		qreal factor;
		
		qreal lastLow;
		qreal lastHigh;
		
		qreal bestBuy;
		qreal bestSell;
		
		HexScanFile(quint32 d, quint32 tts, qreal s, qreal f, qreal l, qreal h) :
			day(d),
			tradeTimeSpot(tts),
			similarity(s),
			factor(f),
			lastLow(l),
			lastHigh(h)
		{
		}
		
		bool operator<(const HexScanFile& other) const
		{
			return (HexScanFile::similarity > other.similarity);
		}
		
		quint32 timeDistance(const HexScanFile& other) const
		{
			if (other.day != HexScanFile::day)
				return 50'000u;
			
			if (other.tradeTimeSpot >= HexScanFile::tradeTimeSpot)
				return other.tradeTimeSpot - HexScanFile::tradeTimeSpot;
			
			return HexScanFile::tradeTimeSpot - other.tradeTimeSpot;
		}
};

class HexScanReport
{
	public:
	
		qreal worstSimilarity;
		quint32 numberOfSamples;
		
		quint32 bWin = 0u;
		quint32 eWin = 0u;
		quint32 sWin = 0u;
		
		std::vector<qreal> bestBuys;
		std::vector<qreal> bestSells;
		
		HexScanReport(qreal ws, quint32 ns) :
			worstSimilarity(ws),
			numberOfSamples(ns)
		{
			HexScanReport::bestBuys.reserve(ns);
			HexScanReport::bestSells.reserve(ns);
		}
};

class HexUpdateFile
{
	public:
	
		std::vector<HexCandlestick<qreal>>	candlesticks;
		qreal					now;
		
		quint32					timeUnit;
		quint32					feedingTimeUnit;
		quint32					numberOfCandlesticks;
		
		quint32					numberOfCandidates;
		qreal					threshold;
		
		qreal					takeProfit;
		qreal					stopLoss;
		
		bool					bestOverGraduates;
		bool					nemesis;
		
		HexUpdateFile(const std::vector<HexCandlestick<qreal>>& candlesticksToBeAnalysed, qreal n, quint32 tu, quint32 ftu, quint32 nc, quint32 cand, qreal thr, qreal tp, qreal sl, bool best, bool nms) :
			now(n),
			timeUnit(tu),
			feedingTimeUnit(ftu),
			numberOfCandlesticks(nc),
			numberOfCandidates(cand),
			threshold(thr),
			takeProfit(tp),
			stopLoss(sl),
			bestOverGraduates(best),
			nemesis(nms)
		{
			const auto numberOfElementaryCandlesticks = tu*nc/ftu;
			auto it = candlesticksToBeAnalysed.cend() - numberOfElementaryCandlesticks;
			HexUpdateFile::candlesticks.reserve(numberOfElementaryCandlesticks);
			
			for (auto i = 0u; i < numberOfElementaryCandlesticks; ++i)
			{
				HexUpdateFile::candlesticks.push_back(*it);
				++it;
			}
		}
};

class HexTerminalFile
{
	public:
	
		quint32		realTime = 0u;
		quint32		timeSpot = ~0u;
		
		qreal		gap = -1.f;
		qreal		volatility = 0.f;
		
		qreal		worstSimilarity = 0.f;
		quint32		numberOfSamples = 0u;
		
		QString		buy;
		qreal 		buy70 = 0.f;
		qreal		buy80 = 0.f;
		qreal		buy90 = 0.f;
		
		quint32		ratio = 0u;
		
		QString		sell;
		qreal		sell70 = 0.f;
		qreal		sell80 = 0.f;
		qreal		sell90 = 0.f;
		
		HexTerminalFile(quint32 rt, quint32 ts, qreal v) : realTime(rt), timeSpot(ts), volatility(static_cast<qreal>(v))
		{
		}
		
		HexTerminalFile(quint32 rt, qreal g, qreal v) : realTime(rt), gap(static_cast<qreal>(g)), volatility(static_cast<qreal>(v))
		{
		}
};

#endif
