#include "ExpandableHashMap.h"
#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream> 
#include <functional>

unsigned int hasher(const GeoCoord& g)
{
	return std::hash<std::string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
	StreetMapImpl();
	~StreetMapImpl();
	bool load(std::string mapFile);
	bool getSegmentsThatStartWith(const GeoCoord& gc, std::vector<StreetSegment>& segs) const;
private:
	ExpandableHashMap<GeoCoord, std::vector<StreetSegment*>> map;
	std::vector<StreetSegment> segs;
	StreetSegment reverse(StreetSegment);
};

StreetMapImpl::StreetMapImpl()
{
}

StreetMapImpl::~StreetMapImpl()
{
}

StreetSegment StreetMapImpl::reverse(StreetSegment input) {
	StreetSegment reversed(input.end, input.start, input.name);
	return reversed;
}

bool StreetMapImpl::load(std::string mapFile)
{
	std::ifstream mapdata(mapFile);
	std::string line;
	while (std::getline(mapdata, line)) {
		std::string streetName = line;
		if (streetName == "")
			break;
		std::getline(mapdata, line);
		std::istringstream count(line);
		int numberCoords;
		count >> numberCoords;
		for (int i = 0; i < numberCoords; i++) {
			std::getline(mapdata, line);
			std::istringstream Coords(line);
			std::string start_latitude, start_longitude, end_latitude, end_longitude;
			Coords >> start_latitude >> start_longitude >> end_latitude >> end_longitude;
			GeoCoord gStart(start_latitude, start_longitude);
			GeoCoord gEnd(end_latitude, end_longitude);
			StreetSegment theSeg(gStart, gEnd, streetName);
			segs.push_back(theSeg);
			StreetSegment reversedSeg = reverse(theSeg);
			segs.push_back(reversedSeg);
		}
	}

	for (int i = 0; i < segs.size(); i++) {
		if (map.find(segs[i].start) == nullptr) {
			std::vector<StreetSegment*> street;
			street.push_back(&segs[i]);
			map.associate(segs[i].start, street);
		}
		else if (map.find(segs[i].start) != nullptr) {
			std::vector<StreetSegment*>* ptrToStreets = map.find(segs[i].start);
			ptrToStreets->push_back(&segs[i]);
		}
	}
	return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, std::vector<StreetSegment>& segs) const
{
	if (map.find(gc) == nullptr) {
		return false;
	}

	if (segs.empty() == false) {
		segs.clear();
	}
	const std::vector<StreetSegment*>* ptrToStreets = map.find(gc);
	for (int i = 0; i < ptrToStreets->size(); i++) {
		segs.push_back(*((*ptrToStreets)[i]));
	}
	return true;
	
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
	m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
	delete m_impl;
}

bool StreetMap::load(std::string mapFile)
{
	return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, std::vector<StreetSegment>& segs) const
{
	return m_impl->getSegmentsThatStartWith(gc, segs);
}
