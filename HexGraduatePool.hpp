#ifndef __GRADUATE_POOL_HPP__
#define __GRADUATE_POOL_HPP__

// Personal Libraries
#include "OtherClasses.hpp"

template<class FileClass>
class HexGraduatePool
{
	private:
	
		const quint32 distance;

		std::vector<FileClass>& negatives;
		std::vector<FileClass>& positives;
	
	public:
		
		inline			HexGraduatePool(std::vector<FileClass>&, std::vector<FileClass>&, quint32);
		inline void		addPositiveGraduate(const FileClass&);
		inline void		addNegativeGraduate(const FileClass&);
};

template<class FileClass>
HexGraduatePool<FileClass>::HexGraduatePool(std::vector<FileClass>& p, std::vector<FileClass>& n, quint32 d) :
	distance(d),
	negatives(n),
	positives(p)
{
}

template<class FileClass>
void HexGraduatePool<FileClass>::addNegativeGraduate(const FileClass& file)
{
	if (HexGraduatePool::negatives.empty() or HexGraduatePool::negatives.back().timeDistance(file) >= HexGraduatePool::distance)
		HexGraduatePool::negatives.push_back(file);
	else if (HexGraduatePool::negatives.back().similarity < file.similarity)
		HexGraduatePool::negatives.back() = file;
}

template<class FileClass>
void HexGraduatePool<FileClass>::addPositiveGraduate(const FileClass& file)
{
	if (HexGraduatePool::positives.empty() or HexGraduatePool::positives.back().timeDistance(file) >= HexGraduatePool::distance)
		HexGraduatePool::positives.push_back(file);
	else if (HexGraduatePool::positives.back().similarity < file.similarity)
		HexGraduatePool::positives.back() = file;
}

#endif
