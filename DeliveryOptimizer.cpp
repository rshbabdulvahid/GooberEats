#include "provided.h"
#include <vector>
using namespace std;

class DeliveryOptimizerImpl
{
public:
	DeliveryOptimizerImpl(const StreetMap* sm);
	~DeliveryOptimizerImpl();
	void optimizeDeliveryOrder(
		const GeoCoord& depot,
		vector<DeliveryRequest>& deliveries,
		double& oldCrowDistance,
		double& newCrowDistance) const;
private:
	void sim_annealing(vector<DeliveryRequest>& deliveries, GeoCoord depot) const;
	double acceptanceProbability(double energy, double newEnergy, double temperature) const;
	double calcDistance(vector <DeliveryRequest> input, GeoCoord depot) const;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

double DeliveryOptimizerImpl::acceptanceProbability(double energy, double newEnergy, double temperature) const {
	if (energy > newEnergy)
		return 1.0;
	else
		return exp((energy - newEnergy) / temperature);
}

double DeliveryOptimizerImpl::calcDistance(vector <DeliveryRequest> input, GeoCoord depot) const {
	double distance = 0;
	distance += distanceEarthMiles(depot, input[0].location);
	for (int i = 1; i < input.size(); i++) {
		distance += distanceEarthMiles(input[i - 1].location, input[i].location);
	}
	distance += distanceEarthMiles(depot, input[input.size() - 1].location);
	return distance;
}

void DeliveryOptimizerImpl::sim_annealing(vector<DeliveryRequest>& deliveries, GeoCoord depot) const {
	double temp = 10000;
	double coolingFactor = 0.003;
	vector<DeliveryRequest> solution = deliveries;
	vector<DeliveryRequest> currentBest = solution;
	while (temp > 1) {
		vector<DeliveryRequest> newPath = solution;
		int randPos1 = rand() % newPath.size();
		int randPos2 = rand() % newPath.size();
		DeliveryRequest swap = newPath[randPos1];
		newPath[randPos1] = newPath[randPos2];
		newPath[randPos2] = swap;
		double currentEnergy = calcDistance(solution, depot);
		double competingEnergy = calcDistance(newPath, depot);
		double randValue = (rand() % 1000) / 1000;
		if (acceptanceProbability(currentEnergy, competingEnergy, temp) > randValue) 
			solution = newPath;
		if (calcDistance(solution, depot) < calcDistance(currentBest, depot))
			currentBest = solution;

		temp *= 1 - coolingFactor;
	}
	deliveries = currentBest;
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
	const GeoCoord& depot,
	vector<DeliveryRequest>& deliveries,
	double& oldCrowDistance,
	double& newCrowDistance) const
{
	if (deliveries.size() == 0)
		return;
	oldCrowDistance = calcDistance(deliveries, depot);
	sim_annealing(deliveries, depot);
	newCrowDistance = calcDistance(deliveries, depot);
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
	m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
	delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
	const GeoCoord& depot,
	vector<DeliveryRequest>& deliveries,
	double& oldCrowDistance,
	double& newCrowDistance) const
{
	return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
