
#ifndef PCM2Classifier_H
#define PCM2Classifier_H

#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <string>

#include "EUVImage.h"
#include "FeatureVector.h"
#include "PCMClassifier.h"

//! Modified Possibilistic C-Means Classifier
/*!
The class implements a modified version of the PCM clustering algorithm.

The modification have been made by Vincent Barra and Cis Verbeeck, to reduce the coincident clusters
problem and he derivation of the eta factors to zero.

*/

class PCM2Classifier : public virtual PCMClassifier
{
	protected :

		using PCMClassifier::computeB;
		void computeU();
		void computeEta();
		void reduceEta();
		
		using PCMClassifier::computeJ;

	public :
		
		//! Constructor
		PCM2Classifier(Real fuzzifier = 2., unsigned numberClasses = 0, Real precision = 0.0015, unsigned maxNumberIteration = 100);

		//! Constructor
		PCM2Classifier(ParameterSection& parameters);

		//! Classification functions
		void classification();
		
		//! Function to initialise the eta
		void initEta(const std::vector<Real>& eta);

};
#endif
