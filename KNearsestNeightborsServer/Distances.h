#ifndef __DISTANCES_H
#define __DISTANCES_H

#include <vector>
#include <memory>

namespace math {
	class Distance {
	public:
		virtual double calculate(const std::vector<double>& first,
								 const std::vector<double>& second) = 0;
	};

	class EuclideanDistance : public Distance {
	public:
		/*
		* Input:
		*	first = The first vector of doubles
		*	second = The second vector of doubles
		* Output:
		*	A double representing the Euclidean distance
		*/
		double calculate(const std::vector<double>& first,
						 const std::vector<double>& second) override;
	};


	class ManhattanDistance : public Distance {
	public:
		/*
		* Input:
		*	first = The first vector of doubles
		*	second = The second vector of doubles
		* Output:
		*	A double representing the Manhattan distance
		*/
		double calculate(const std::vector<double>& first,
						 const std::vector<double>& second) override;
	};

	class ChebyshevDistance : public Distance {
	public:
		/*
		* Input:
		*	first = The first vector of doubles
		*	second = The second vector of doubles
		* Output:
		*	A double representing the Chebyshev distance
		*/
		double calculate(const std::vector<double>& first,
						 const std::vector<double>& second) override;
	};

	class CanberraDistance : public Distance {
	public:
		/*
		* Input:
		*	first = The first vector of doubles
		*	second = The second vector of doubles
		* Output:
		*	A double representing the Canberra distance
		*/
		double calculate(const std::vector<double>& first,
						 const std::vector<double>& second) override;
	};

	class MinkowskiDistance : public Distance {
	public:
		/*
		* Input:
		*	first = The first vector of doubles
		*	second = The second vector of doubles
		* Output:
		*	A double representing the Minkowski distance
		*/
		double calculate(const std::vector<double>& first,
						 const std::vector<double>& second) override;
	};

	class DistanceFactory {
	public:
		enum class DistanceType {
			Euclidean,
			Manhattan,
			Chebyshev,
			Canberra,
			Minkowski
		};

		static std::unique_ptr<Distance> createDistance(DistanceType distanceType);
	};
}

#endif