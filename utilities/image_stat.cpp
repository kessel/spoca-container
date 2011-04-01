#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <fenv.h>
#include <iomanip>

#include "../classes/tools.h"
#include "../classes/constants.h"

#include "../classes/Image.h"
#include "../classes/EUVImage.h"
#include "../classes/ArgumentHelper.h"
#include "../classes/mainutilities.h"

#ifndef Real
#define Real float
#endif

using namespace std;
using namespace dsr;

string outputFileName;

extern template class Image<Real>;

int main(int argc, const char **argv)
{
	#if DEBUG >= 1
	feenableexcept(FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
	cout<<setiosflags(ios::fixed);
	#endif

	// The list of names of the sun images to process
	string imageType = "UNKNOWN";
	vector<string> imagesFilenames;

	// Options for the preprocessing of images
	string preprocessingSteps = "NAR";
	double radiusRatio = 1.31;

		
	// Option for the size of the neighboorhood
	unsigned neighboorhoodRadius = 1;

	string programDescription = "This Program generates neigboorhood stats fits files.\n";
	programDescription+="Compiled with options :";
	programDescription+="\nNUMBERWAVELENGTH: " + itos(NUMBERWAVELENGTH);
	programDescription+="\nDEBUG: "+ itos(DEBUG);
	programDescription+="\nPixelType: " + string(typeid(PixelType).name());
	programDescription+="\nReal: " + string(typeid(Real).name());

	ArgumentHelper arguments;
	arguments.new_named_string('I', "imageType","string", "\n\tThe type of the images.\n\tPossible values are : EIT, EUVI, AIA, SWAP\n\t", imageType);
	arguments.new_named_string('P', "preprocessingSteps", "comma separated list of string (no spaces)", "\n\tThe steps of preprocessing to apply to the sun images.\n\tPossible values :\n\t\tNAR (Nullify above radius)\n\t\tALC (Annulus Limb Correction)\n\t\tDivMedian (Division by the median)\n\t\tTakeSqrt (Take the square root)\n\t\tTakeLog (Take the log)\n\t\tDivMode (Division by the mode)\n\t\tDivExpTime (Division by the Exposure Time)\n\t", preprocessingSteps);
	arguments.new_named_double('r', "radiusratio", "positive real", "\n\tThe ratio of the radius of the sun that will be processed.\n\t",radiusRatio);
	arguments.new_named_unsigned_int('N', "neighboorhoodRadius", "positive integer", "\n\tOnly for spatial operation.\n\tThe neighboorhoodRadius is half the size of the disc of neighboors, for example with a value of 1, the disc has a diameter of 3.\n\t", neighboorhoodRadius);
	arguments.set_string_vector("fitsFileName1 fitsFileName2 ...", "\n\tThe name of the fits files containing the images of the sun.\n\t", imagesFilenames);
	arguments.set_description(programDescription.c_str());
	arguments.set_author("Benjamin Mampaey, benjamin.mampaey@sidc.be");
	arguments.set_build_date(__DATE__);
	arguments.set_version("1.0");
	arguments.process(argc, argv);

	if(imagesFilenames.size() == 0)
	{
		cerr<<imagesFilenames.size()<<" fits image file given as parameter, at least 1 must be given!"<<endl;
		return EXIT_FAILURE;
	}
	
	EUVImage stat;

	for (unsigned p = 0; p< imagesFilenames.size(); ++p)
	{

		EUVImage* image  = getImageFromFile(imageType, imagesFilenames[p]);
		image->preprocessing(preprocessingSteps,radiusRatio);
		
		stat.copyKeywords(image);
		
		outputFileName = stripSuffix(imagesFilenames[p]) + ".N" + itos(neighboorhoodRadius) + ".";
		
		stat.neighboorhoodMean(image, neighboorhoodRadius);
		stat.writeFits(outputFileName + "neighboorhoodMean.fits");
		
		stat.neighboorhoodVariance(image, neighboorhoodRadius);
		stat.writeFits(outputFileName + "neighboorhoodVariance.fits");
		
		stat.neighboorhoodSkewness(image, neighboorhoodRadius);
		stat.writeFits(outputFileName + "neighboorhoodSkewness.fits");
		
		stat.neighboorhoodKurtosis(image, neighboorhoodRadius);
		stat.writeFits( outputFileName + "neighboorhoodKurtosis.fits");
		
		stat.sobel(image);
		stat.writeFits( outputFileName + "sobel.fits");
		
		stat.sobel_approx(image);
		stat.writeFits( outputFileName + "sobel_approx.fits");
		
		delete image;
	

	}

	return EXIT_SUCCESS;
}
