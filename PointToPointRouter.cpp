#include "provided.h"
#include "ExpandableHashMap.h"
#include <unordered_set>
#include <queue>
#include <cmath>
#include <list>
using namespace std;

class PointToPointRouterImpl
{
public:
	PointToPointRouterImpl(const StreetMap* sm);
	~PointToPointRouterImpl();
	DeliveryResult generatePointToPointRoute(
		const GeoCoord& start,
		const GeoCoord& end,
		list<StreetSegment>& route,
		double& totalDistanceTravelled) const;
private:
	const StreetMap* m_streetMap;
	struct CoordVal {
		GeoCoord g;
		double fvalue;
		double gvalue;
	};
	struct compare {
		bool operator()(const CoordVal& c1, const CoordVal& c2)
		{
			return (c1.fvalue > c2.fvalue);
		}
	};
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
	m_streetMap = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}


DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
	const GeoCoord& start,
	const GeoCoord& end,
	list<StreetSegment>& route,
	double& totalDistanceTravelled) const
{
	ExpandableHashMap<GeoCoord, GeoCoord> m_routeTracker;
	priority_queue<CoordVal, vector<CoordVal>, compare> openList;
	ExpandableHashMap<GeoCoord, double> openListContents;
	ExpandableHashMap<GeoCoord, double> closedList;
	//checking coord validity
	vector<StreetSegment> temp;
	if (m_streetMap->getSegmentsThatStartWith(start, temp) == false || m_streetMap->getSegmentsThatStartWith(end, temp) == false) {
		return BAD_COORD;
	}
	//if start is end, we are done
	if (start == end) {
		route.clear();
		return DELIVERY_SUCCESS;
	}
	//push starting point with f = h(start_point)
	CoordVal sp;
	sp.g = start;
	sp.fvalue = 0;
	sp.gvalue = 0;
	openList.push(sp);
	openListContents.associate(sp.g, 0);
	while (!openList.empty()) {
		CoordVal current = openList.top();
		openList.pop();
		openListContents.associate(current.g, -100);
		//generating successors
		std::vector<StreetSegment> successorSegments;
		m_streetMap->getSegmentsThatStartWith(current.g, successorSegments);
		CoordVal* successors = new CoordVal[successorSegments.size()];
		for (int i = 0; i < successorSegments.size(); i++) {
			successors[i].g = successorSegments[i].end;
		}
		//MAIN FOR LOOP
		for (int i = 0; i < successorSegments.size(); i++) {
			if (successors[i].g == end) {
				m_routeTracker.associate(successors[i].g, current.g);
				route.clear();
				delete[] successors;
				GeoCoord g = end;
				while (m_routeTracker.find(g) != nullptr) {
					GeoCoord* next = m_routeTracker.find(g);
					totalDistanceTravelled += distanceEarthMiles(*next, g);
					StreetSegment s;
					vector<StreetSegment> segments;
					m_streetMap->getSegmentsThatStartWith(*next, segments);
					for (int i = 0; i < segments.size(); i++) {
						if (segments[i].end == g)
							s = segments[i];
					}
					route.push_front(s);
					g = *next;
				}
				return DELIVERY_SUCCESS;
			}
			successors[i].gvalue = current.gvalue + distanceEarthMiles(current.g, successors[i].g);
			successors[i].fvalue = successors[i].gvalue + distanceEarthMiles(successors[i].g, end);
			if (!openList.empty()) {
				if (openListContents.find(successors[i].g) != nullptr) {
					double* fComp = openListContents.find(successors[i].g);
					if (*fComp >= 0 && *fComp < successors[i].fvalue)
						continue;
				}
			}
			if (closedList.find(successors[i].g) != nullptr) {
				double* fComp = closedList.find(successors[i].g);
				if (*fComp < successors[i].fvalue)
					continue;
			}
			openList.push(successors[i]);
			openListContents.associate(successors[i].g, successors[i].fvalue);
			m_routeTracker.associate(successors[i].g, current.g);
		}
		delete[] successors;
		closedList.associate(current.g, current.fvalue);
	}

	return NO_ROUTE;
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
	m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
	delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
	const GeoCoord& start,
	const GeoCoord& end,
	list<StreetSegment>& route,
	double& totalDistanceTravelled) const
{
	return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
