#include "HistogramPCM2Classifier.h"

using namespace std;

HistogramPCM2Classifier::HistogramPCM2Classifier(Real fuzzifier, unsigned numberClasses, Real precision, unsigned maxNumberIteration, const RealFeature& binSize)
:FCMClassifier(fuzzifier, numberClasses, precision, maxNumberIteration), PCMClassifier(fuzzifier, numberClasses, precision, maxNumberIteration), HistogramPCMClassifier(fuzzifier, numberClasses, precision, maxNumberIteration, binSize)
{
	#if defined DEBUG
	cout<<"Called HPCM2 constructor"<<endl;
	#endif
}

HistogramPCM2Classifier::HistogramPCM2Classifier(ParameterSection& parameters)
:FCMClassifier(parameters), PCMClassifier(parameters), HistogramPCMClassifier(parameters)
{
	#if defined DEBUG
	cout<<"Called HPCM2 constructor with parameter section"<<endl;
	#endif
}

void HistogramPCM2Classifier::attribution()
{
	PCM2Classifier::sortB();
	PCM2Classifier::computeU();
}


void HistogramPCM2Classifier::computeU()
{
	U.resize(numberBins * numberClasses);
	
	MembershipSet::iterator uij = U.begin();
	if (fuzzifier == 1.5)
	{
		for (HistoFeatureVectorSet::iterator xj = HistoX.begin(); xj != HistoX.end(); ++xj)
		{
			for (unsigned i = 0 ; i < numberClasses ; ++i, ++uij)
			{
				*uij = distance_squared(*xj,B[i]) / eta[i] ;
				*uij *= *uij;
				*uij = 1. / (1. + *uij * *uij);
			}
		}
	}
	else if (fuzzifier == 2)
	{
		for (HistoFeatureVectorSet::iterator xj = HistoX.begin(); xj != HistoX.end(); ++xj)
		{
			for (unsigned i = 0 ; i < numberClasses ; ++i, ++uij)
			{
				*uij = distance_squared(*xj,B[i]) / eta[i] ;
				*uij = 1. / (1. + *uij * *uij);
			}
		}
	}
	else
	{
		for (HistoFeatureVectorSet::iterator xj = HistoX.begin(); xj != HistoX.end(); ++xj)
		{
			for (unsigned i = 0 ; i < numberClasses ; ++i, ++uij)
			{
				*uij = distance_squared(*xj,B[i]) / eta[i] ;
				*uij = 1. / (1. + pow(*uij , Real(2./(fuzzifier-1.))));
			}
		}
	}
}


// VERSION WITH LIMITED VARIATION OF ETA W.R.T. ITS INITIAL VALUE

void HistogramPCM2Classifier::classification()
{

	if(HistoX.size() == 0 || B.size() == 0 || B.size() != eta.size())
	{
		cerr<<"Error : The Classifier must be initialized before doing classification."<<endl;
		exit(EXIT_FAILURE);
	
	}
	
	#if defined EXTRA_SAFE
	int excepts = feenableexcept(FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
	cout<<setiosflags(ios::fixed);
	#endif
	
	#if defined VERBOSE
	cout<<"--HistogramPCM2Classifier::classification--START--"<<endl;
	#endif
	
	stepinit(filenamePrefix+"iterations.txt");
	
	const Real maxFactor = ETA_MAXFACTOR;
	
	//Initialisation of precision & U
	this->precision = precision;
	
	Real precisionReached = numeric_limits<Real>::max();
	vector<RealFeature> oldB = B;
	vector<Real> start_eta = eta;
	bool recomputeEta = FIXETA != true;
	for (unsigned iteration = 0; iteration < maxNumberIteration && precisionReached > precision ; ++iteration)
	{
	
		if (recomputeEta)	//eta is to be recalculated each iteration.
		{
			computeEta();
			for (unsigned i = 0 ; i < numberClasses && recomputeEta ; ++i)
			{
				if ( (start_eta[i] / eta[i] > maxFactor) || (start_eta[i] / eta[i] < 1. / maxFactor) )
				{
					recomputeEta = false;
				}
			}
		}
		
		computeU();
		computeB();
		
		precisionReached = variation(oldB,B);
		
		// avoid class cannibalism
		if (precisionReached <= precision)
		{
			reduceEta();
			computeU();
			computeB();
		}
		
		oldB = B;
		
		stepout(iteration, precisionReached, precision);
	}
	
	#if defined VERBOSE
	cout<<endl<<"--HistogramPCM2Classifier::classification--END--"<<endl;
	#endif
	
	#if defined EXTRA_SAFE
	feenableexcept(excepts);
	#endif
}



void HistogramPCM2Classifier::computeEta()
{
	HistogramPCMClassifier::computeEta();

	#if defined VERBOSE
		cout<<"\npre_eta:\t"<<eta<<"\t";
	#endif

	PCM2Classifier::reduceEta();

}
