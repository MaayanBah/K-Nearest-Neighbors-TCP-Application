#include "Distances.h"
#include <utility>
#include <cstdlib>
#include <cmath>
#include <stdexcept>

using namespace std;

/*
* Input:
*	first = The first vector of doubles
*	second = The second vector of doubles
* Output:
*	A double representing the Euclidean distance
*/
double math::EuclideanDistance::calculate(const vector<double>& first,
										  const vector<double>& second) {
	double sum = 0;

	for (unsigned int index = 0; index < first.size(); index++) {
		sum += pow(abs(first[index] - second[index]), 2);
	}

	return sqrt(sum);
}

/*
* Input:
*	first = The first vector of doubles
*	second = The second vector of doubles
* Output:
*	A double representing the Manhattan distance
*/
double math::ManhattanDistance::calculate(const vector<double>& first,
										  const vector<double>& second) {
	double sum = 0;

	for (unsigned int index = 0; index < first.size(); index++) {
		sum += (abs(first[index] - second[index]));
	}

	return sum;
}

/*
* Input:
*	first = The first vector of doubles
*	second = The second vector of doubles
* Output:
*	A double representing the Chebyshev distance
*/
double math::ChebyshevDistance::calculate(const vector<double>& first,
										  const vector<double>& second) {
	double max = 0;

	for (unsigned int index = 0; index < first.size(); index++) {
		max = std::max<double>(max, abs(first[index] - second[index]));
	}

	return max;
}

/*
* Input:
*	first = The first vector of doubles
*	second = The second vector of doubles
* Output:
*	A double representing the Canberra distance
*/
double math::CanberraDistance::calculate(const vector<double>& first,
										 const vector<double>& second) {
	double sum = 0;

	for (unsigned int index = 0; index < first.size(); index++) {
		double denominator = (abs(first[index]) + abs(second[index]));

		if (denominator == 0) {
			throw runtime_error("Tried dividing by zero in Canberra!");
		}

		sum += abs(first[index] - second[index]) / denominator;
	}

	return sum;
}

/*
* Input:
*	first = The first vector of doubles
*	second = The second vector of doubles
* Output:
*	A double representing the Minkowski distance
*/
double math::MinkowskiDistance::calculate(const vector<double>& first,
										  const vector<double>& second) {
	double sum = 0;

	for (unsigned int index = 0; index < first.size(); index++) {
		sum += pow(abs(first[index] - second[index]), 2.0);
	}

	return pow(sum, 1 / 2.0);
}

/*
* Input:
*	distanceType = The distance type by which to create the distance method
* Output:
*	The matching distance by the distance type.
*/
std::unique_ptr<math::Distance> math::DistanceFactory::createDistance(DistanceType distanceType) {
	switch (distanceType) {
		case DistanceType::Euclidean: return std::unique_ptr<EuclideanDistance>(new EuclideanDistance());
		case DistanceType::Manhattan: return std::unique_ptr<ManhattanDistance>(new ManhattanDistance());
		case DistanceType::Chebyshev: return std::unique_ptr<ChebyshevDistance>(new ChebyshevDistance());
		case DistanceType::Canberra: return std::unique_ptr<CanberraDistance>(new CanberraDistance());
		case DistanceType::Minkowski: return std::unique_ptr<MinkowskiDistance>(new MinkowskiDistance());
	}

	return nullptr;
}

std::string math::DistanceFactory::distanceName(DistanceType distanceType) {
	switch (distanceType) {
		case DistanceType::Euclidean: return "AUC";
		case DistanceType::Manhattan: return "MAN";
		case DistanceType::Chebyshev: return "CHB";
		case DistanceType::Canberra: return "CAN";
		case DistanceType::Minkowski: return "MIN";
	}
	
	return "";
}
