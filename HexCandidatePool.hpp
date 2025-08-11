#ifndef __CANDIDATE_POOL_HPP__
#define __CANDIDATE_POOL_HPP__

// Personal Libraries
#include "OtherClasses.hpp"

template<class FileClass>
class HexCandidatePool
{
	private:
	
		const quint32			distance;
		const quint32			numberOfCandidates;
		
		std::vector<FileClass>&		negatives;
		quint32				nLatest = std::numeric_limits<quint32>::max();
		quint32				nWorst = std::numeric_limits<quint32>::max();
		qreal				nBottom = 2.;
		
		std::vector<FileClass>&		positives;
		quint32				pLatest = std::numeric_limits<quint32>::max();
		quint32				pWorst = std::numeric_limits<quint32>::max();
		qreal				pBottom = 2.;
		
		inline static void		UpdateInfo(const std::vector<FileClass>&, qreal&, quint32&, quint32&);
	
	public:
		
		inline				HexCandidatePool(std::vector<FileClass>&, std::vector<FileClass>&, quint32, quint32);
		inline void			addPositiveCandidate(const FileClass&);
		inline void			addNegativeCandidate(const FileClass&);
		inline quint32			numberOfResults(void) const;
		inline qreal			worstSimilarity(void) const;
};

template<class FileClass>
HexCandidatePool<FileClass>::HexCandidatePool(std::vector<FileClass>& p, std::vector<FileClass>& n, quint32 d, quint32 nc) :
	distance(d),
	numberOfCandidates(nc),
	negatives(n),
	positives(p)
{
}

template<class FileClass>
void HexCandidatePool<FileClass>::addNegativeCandidate(const FileClass& file)
{
	auto somethingChanged = false;
	
	if (HexCandidatePool::numberOfResults() < HexCandidatePool::numberOfCandidates)
	{
		if (HexCandidatePool::negatives.empty() or HexCandidatePool::negatives[HexCandidatePool::nLatest].timeDistance(file) >= HexCandidatePool::distance)
		{
			HexCandidatePool::negatives.push_back(file);
			somethingChanged = true;
		}
		else if (HexCandidatePool::negatives[HexCandidatePool::nLatest].similarity < file.similarity)
		{
			HexCandidatePool::negatives[HexCandidatePool::nLatest] = file;
			somethingChanged = true;
		}
	}
	else if (HexCandidatePool::negatives.size() != 0u and HexCandidatePool::negatives[HexCandidatePool::nLatest].timeDistance(file) < HexCandidatePool::distance)
	{
		if (HexCandidatePool::negatives[HexCandidatePool::nLatest].similarity < file.similarity)
		{
			HexCandidatePool::negatives[HexCandidatePool::nLatest] = file;
			somethingChanged = true;
		}
	}
	else if (HexCandidatePool::nBottom <= HexCandidatePool::pBottom)
	{
		HexCandidatePool::negatives[HexCandidatePool::nWorst] = file;
		somethingChanged = true;
	}
	else
	{
		HexCandidatePool::positives.erase(HexCandidatePool::positives.begin() + HexCandidatePool::pWorst);
		HexCandidatePool::UpdateInfo(HexCandidatePool::positives, HexCandidatePool::pBottom, HexCandidatePool::pLatest, HexCandidatePool::pWorst);
		
		HexCandidatePool::negatives.push_back(file);
		somethingChanged = true;
	}
	
	if (somethingChanged)
		HexCandidatePool::UpdateInfo(HexCandidatePool::negatives, HexCandidatePool::nBottom, HexCandidatePool::nLatest, HexCandidatePool::nWorst);
}

template<class FileClass>
void HexCandidatePool<FileClass>::addPositiveCandidate(const FileClass& file)
{
	auto somethingChanged = false;
	
	if (HexCandidatePool::numberOfResults() < HexCandidatePool::numberOfCandidates)
	{
		if (HexCandidatePool::positives.empty() or HexCandidatePool::positives[HexCandidatePool::pLatest].timeDistance(file) >= HexCandidatePool::distance)
		{
			HexCandidatePool::positives.push_back(file);
			somethingChanged = true;
		}
		else if (HexCandidatePool::positives[HexCandidatePool::pLatest].similarity < file.similarity)
		{
			HexCandidatePool::positives[HexCandidatePool::pLatest] = file;
			somethingChanged = true;
		}
	}
	else if (HexCandidatePool::positives.size() != 0u and HexCandidatePool::positives[HexCandidatePool::pLatest].timeDistance(file) < HexCandidatePool::distance)
	{
		if (HexCandidatePool::positives[HexCandidatePool::pLatest].similarity < file.similarity)
		{
			HexCandidatePool::positives[HexCandidatePool::pLatest] = file;
			somethingChanged = true;
		}
	}
	else if (HexCandidatePool::nBottom <= HexCandidatePool::pBottom)
	{
		HexCandidatePool::negatives.erase(HexCandidatePool::negatives.begin() + HexCandidatePool::nWorst);
		HexCandidatePool::UpdateInfo(HexCandidatePool::negatives, HexCandidatePool::nBottom, HexCandidatePool::nLatest, HexCandidatePool::nWorst);
		
		HexCandidatePool::positives.push_back(file);
		somethingChanged = true;
	}
	else
	{
		HexCandidatePool::positives[HexCandidatePool::pWorst] = file;
		somethingChanged = true;
	}
	
	if (somethingChanged)
		HexCandidatePool::UpdateInfo(HexCandidatePool::positives, HexCandidatePool::pBottom, HexCandidatePool::pLatest, HexCandidatePool::pWorst);
}

template<class FileClass>
quint32 HexCandidatePool<FileClass>::numberOfResults(void) const
{
	return HexCandidatePool::negatives.size() + HexCandidatePool::positives.size();
}

template<class FileClass>
void HexCandidatePool<FileClass>::UpdateInfo(const std::vector<FileClass>& results, qreal& bottom, quint32& latest, quint32& worst)
{
	latest = std::numeric_limits<quint32>::max();
	worst = std::numeric_limits<quint32>::max();
	bottom = 2.;
	
	auto maxTimeSpot = 0u;
	auto count = 0u;
	
	for (const auto& file : results)
	{
		if (file.similarity < bottom)
		{
			bottom = file.similarity;
			worst = count;
		}
		
		const auto time = 50'000u*file.day + file.tradeTimeSpot;
		
		if (time > maxTimeSpot)
		{
			maxTimeSpot = time;
			latest = count;
		}
		
		++count;
	}
}

template<class FileClass>
qreal HexCandidatePool<FileClass>::worstSimilarity(void) const
{
	if (HexCandidatePool::pBottom < HexCandidatePool::nBottom)
		return HexCandidatePool::pBottom;
	
	return HexCandidatePool::nBottom;
}

#endif
