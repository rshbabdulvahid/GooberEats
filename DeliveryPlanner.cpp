#include "provided.h"
#include "ExpandableHashMap.h"
#include <vector>
#include <list>

using namespace std;

class DeliveryPlannerImpl
{
public:
	DeliveryPlannerImpl(const StreetMap* sm);
	~DeliveryPlannerImpl();
	DeliveryResult generateDeliveryPlan(
		const GeoCoord& depot,
		const vector<DeliveryRequest>& deliveries,
		vector<DeliveryCommand>& commands,
		double& totalDistanceTravelled) const;
private:
	const StreetMap* m_streetMap;
	DeliveryOptimizer m_optimizer;
	PointToPointRouter m_router;
	void convertToCommands(vector<DeliveryCommand>& commands, list<StreetSegment> thePath) const;
	string computeDirection(double angle) const;
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
	:m_streetMap(sm), m_optimizer(m_streetMap), m_router(m_streetMap)
{
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

string DeliveryPlannerImpl::computeDirection(double angle) const {
	if (angle >= 0 && angle < 22.5)
		return "east";
	if (angle >= 22.5 && angle < 67.5)
		return "northeast";
	if (angle >= 67.5 && angle < 112.5)
		return "north";
	if (angle >= 112.5 && angle < 157.5)
		return "northwest";
	if (angle >= 157.5 && angle < 202.5)
		return "west";
	if (angle >= 202.5 && angle < 247.5)
		return "southwest";
	if (angle >= 247.5 && angle < 292.5)
		return "south";
	if (angle >= 292.5 && angle < 337.5)
		return "southeast";
	return "east";
}

void DeliveryPlannerImpl::convertToCommands(vector<DeliveryCommand>& commands, list<StreetSegment> thePath) const {
	list<StreetSegment>::iterator it = thePath.begin();
	string currStreet = it->name;
	string angle = computeDirection(angleOfLine(*it));
	double travelDist = 0;
	while (it != thePath.end()) {
		while (it != thePath.end() && it->name == currStreet) {
			travelDist += distanceEarthMiles(it->start, it->end);
			it++;
		}
		DeliveryCommand currCommand;
		currCommand.initAsProceedCommand(angle, currStreet, travelDist);
		commands.push_back(currCommand);
		if (it != thePath.end()) {
			list<StreetSegment>::iterator temp = it;
			it--;
			if (angleBetween2Lines(*it, *temp) >= 1 && angleBetween2Lines(*it, *temp) < 180) {
				DeliveryCommand turn;
				turn.initAsTurnCommand("left", temp->name);
				commands.push_back(turn);
			}
			else if (angleBetween2Lines(*it, *temp) >= 180 && angleBetween2Lines(*it, *temp) <= 359) {
				DeliveryCommand turn;
				turn.initAsTurnCommand("right", temp->name);
				commands.push_back(turn);
			}
			currStreet = temp->name;
			angle = computeDirection(angleOfLine(*temp));
			travelDist = 0;
			it++;
		}
	}
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
	const GeoCoord& depot,
	const vector<DeliveryRequest>& deliveries,
	vector<DeliveryCommand>& commands,
	double& totalDistanceTravelled) const
{
	if (deliveries.size() == 0)
		return NO_ROUTE;
	totalDistanceTravelled = 0;
	double old, newDist;
	vector<DeliveryRequest> newlyOrdered = deliveries;
	m_optimizer.optimizeDeliveryOrder(depot, newlyOrdered, old, newDist);
	vector<list<StreetSegment>> routes;
	list<StreetSegment> pathBetween;
	DeliveryResult resultHolder;
	//path between depot and the first delivery location
	resultHolder = m_router.generatePointToPointRoute(depot, newlyOrdered[0].location, pathBetween, totalDistanceTravelled);
	if (resultHolder == DELIVERY_SUCCESS)
		routes.push_back(pathBetween);
	else if (resultHolder == BAD_COORD)
		return BAD_COORD;
	else
		return NO_ROUTE;
	//path between each depot added to vector of paths
	for (int i = 1; i < newlyOrdered.size(); i++) {
		resultHolder = m_router.generatePointToPointRoute(newlyOrdered[i - 1].location, newlyOrdered[i].location, pathBetween, totalDistanceTravelled);
		if (resultHolder == DELIVERY_SUCCESS)
			routes.push_back(pathBetween);
		else if (resultHolder == BAD_COORD)
			return BAD_COORD;
		else
			return NO_ROUTE;
	}
	//path from last delivery back to depot
	m_router.generatePointToPointRoute(newlyOrdered[newlyOrdered.size() - 1].location, depot, pathBetween, totalDistanceTravelled);
	routes.push_back(pathBetween);
	//Now creating the delivery commands
	for (int i = 0; i < routes.size() - 1; i++) {
		convertToCommands(commands, routes[i]);
		DeliveryCommand delivery;
		delivery.initAsDeliverCommand(newlyOrdered[i].item);
		commands.push_back(delivery);
	}
	//Command to navigate back to depot
	convertToCommands(commands, routes[routes.size() - 1]);
	return DELIVERY_SUCCESS;
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
	m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
	delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
	const GeoCoord& depot,
	const vector<DeliveryRequest>& deliveries,
	vector<DeliveryCommand>& commands,
	double& totalDistanceTravelled) const
{
	return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
