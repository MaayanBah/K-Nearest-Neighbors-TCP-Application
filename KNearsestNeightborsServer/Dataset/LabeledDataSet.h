#ifndef __LABELED_DATA_SET_H
#define __LABELED_DATA_SET_H

#include <vector>
#include <string>
#include "Dataset/Distances.h"

namespace data {
	class LabeledDataSet {
	public:
		/*
		* This class represents a single row in the classified data set. 
		* It consists of features and a label.
		*/
		struct LabeledData {
			// Data Members
			std::vector<double> features;
			std::string label;
		};

		// Special Constructors
		LabeledDataSet() = default;
		LabeledDataSet(const LabeledDataSet&) = default;
		LabeledDataSet(LabeledDataSet&&) = default;

		// Assignment Operators
		LabeledDataSet& operator=(const LabeledDataSet&) = default;
		LabeledDataSet& operator=(LabeledDataSet&&) = default;

		// Destructor
		~LabeledDataSet() = default;

		// Constructor that parses the file
		LabeledDataSet(std::vector<LabeledData> dataSet);

		// Methods
		inline unsigned int size() const { return dataSet.size(); }
		inline unsigned int featureSize() const { return dataSet[0].features.size(); }
		std::string predictLabel(const std::vector<double> vectorToCategorize,
								 math::Distance& distanceMethod,
								 int k) const;

	private:
		// Data Members
		std::vector<LabeledData> dataSet;
	};
}

#endif
