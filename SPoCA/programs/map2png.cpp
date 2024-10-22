//! This program convert a color map FITS file to a png image.
/*!
@page map2png map2png.x

Version: 3.0

Author: Benjamin Mampaey, benjamin.mampaey@sidc.be

@section usage Usage
<tt> bin/map2png.x [-option optionvalue ...] fitsFile</tt>

@param fitsFile	Path to the color map FITS file to be converted

global parameters:

@param help	Print a help message and exit.
<BR>If you pass the value doxygen, the help message will follow the doxygen convention.
<BR>If you pass the value config, the help message will write a configuration file template.

@param config	Program option configuration file.

@param colors	The list of color of the regions to plot separated by commas or a file containg such a list. All regions will be selected if ommited.

@param fill	Set this flag if you want to fill holes in the regions before ploting.

@param upperLabel	The label to write on the upper left corner.
<BR>If set but no value is passed, a default label will be written.
<BR>You can use keywords from the color map fits file by specifying them between {}

@param lowerLabel	The label to write on the lower left corner.
<BR>You can use keywords from the color map fits file by specifying them between {}

@param output	The path of the output file or of a directory.

@param recenter	Set to the position of the new sun center if you want to translate the image

@param scaling	Set to the scaling factor if you want to rescale the image.

@param size	The size of the image written. i.e. "1024x1024". See ImageMagick Image Geometry for specification.
<BR>If not set the output image will have the same dimension as the input image.

@param straightenUp	Set if you want to rotate the image so the solar north is up.

@param transparent	If you want the null values to be transparent.

@param uniqueColor	Set to a color if you want all regions to be plotted in that color.
<BR>See gradient image for the color number.

See @ref Compilation_Options for constants and parameters at compilation time.

*/

#include <vector>
#include <iostream>
#include <string>
#include <set>

#include "../classes/tools.h"
#include "../classes/constants.h"
#include "../classes/mainutilities.h"
#include "../classes/ArgParser.h"

#include "../classes/ColorMap.h"
#include "../classes/MagickImage.h"

using namespace std;

using Magick::Color;
using Magick::ColorGray;
using Magick::Geometry;
using Magick::Quantum;

string filenamePrefix;

int main(int argc, const char **argv)
{
	// We declare our program description
	string programDescription = "This program convert a color map FITS file to a png image.";
	programDescription+="\nVersion: 3.0";
	programDescription+="\nAuthor: Benjamin Mampaey, benjamin.mampaey@sidc.be";
	
	programDescription+="\nCompiled on "  __DATE__  " with options :";
	programDescription+="\nNUMBERCHANNELS: " + toString(NUMBERCHANNELS);
	#if defined DEBUG
	programDescription+="\nDEBUG: ON";
	#endif
	#if defined EXTRA_SAFE
	programDescription+="\nEXTRA_SAFE: ON";
	#endif
	#if defined VERBOSE
	programDescription+="\nVERBOSE: ON";
	#endif
	programDescription+="\nEUVPixelType: " + string(typeid(EUVPixelType).name());
	programDescription+="\nReal: " + string(typeid(Real).name());

	// We define our program parameters
	ArgParser args(programDescription);
	
	args["config"] = ArgParser::ConfigurationFile('C');
	args["help"] = ArgParser::Help('h');
	
	args["upperLabel"] = ArgParser::Parameter("", 'L', "The label to write on the upper left corner.\nIf set but no value is passed, a default label will be written.\nYou can use keywords from the color map fits file by specifying them between {}");
	args["lowerLabel"] = ArgParser::Parameter("{CLASTYPE} {CPREPROC}", 'l', "The label to write on the lower left corner.\nYou can use keywords from the color map fits file by specifying them between {}");
	args["fill"] = ArgParser::Parameter(false, 'f', "Set this flag if you want to fill holes in the regions before ploting.");
	args["colors"] = ArgParser::Parameter("", 'c', "The list of color of the regions to plot separated by commas or a file containg such a list. All regions will be selected if ommited.");
	args["uniqueColor"] = ArgParser::Parameter(7, 'U', "Set to a color if you want all regions to be plotted in that color.\nSee gradient image for the color number.");
	args["transparent"] = ArgParser::Parameter(false, 't', "If you want the null values to be transparent.");
	args["straightenUp"] = ArgParser::Parameter(false, 'u', "Set if you want to rotate the image so the solar north is up.");
	args["recenter"] = ArgParser::Parameter("", 'R', "Set to the position of the new sun center if you want to translate the image");
	args["scaling"] = ArgParser::Parameter(1, 's', "Set to the scaling factor if you want to rescale the image.");
	args["size"] = ArgParser::Parameter("100%x100%", 'S', "The size of the image written. i.e. \"1024x1024\". See ImageMagick Image Geometry for specification.\nIf not set the output image will have the same dimension as the input image.");
	args["output"] = ArgParser::Parameter(".", 'O', "The path of the output file or of a directory.");
	args["fitsFile"] = ArgParser::PositionalParameter("Path to the color map FITS file to be converted");
	
	// We parse the arguments
	try
	{
		args.parse(argc, argv);
	}
	catch(const invalid_argument& error)
	{
		cerr<<"Error : "<<error.what()<<endl;
		cerr<<args.help_message(argv[0])<<endl;
		return EXIT_FAILURE;
	}
	
	// We setup the filenamePrefix
	if (isDir(args["output"]))
	{
		filenamePrefix = stripSuffix(stripPath(args["fitsFile"]));
		filenamePrefix = makePath(args["output"], filenamePrefix + ".");
	}
	else
	{
		string outputDirectory = getPath(args["output"]);
		if (! isDir(outputDirectory))
		{
			cerr<<"Error : "<<outputDirectory<<" is not a directory!"<<endl;
			return EXIT_FAILURE;
		}
		filenamePrefix = stripSuffix(args["output"]) + ".";
	}

	// We parse the size option
	Magick::Geometry size_geometry(args["size"].as<string>());
	if(!size_geometry.isValid())
	{
		cerr << "Error: Size parameter "<<args["size"]<<" is not a valid specification."<< endl;
		return EXIT_FAILURE;
	}
	
	// We parse the background color
	Color backgroundColor(0, 0 ,0, 0);
	if(args["transparent"])
	{
		backgroundColor.alpha(1);
	}
	else
	{
		backgroundColor.alpha(0);
	}
	
	// We parse the colors of the regions to plot
	set<ColorType> colors;
	if(args["colors"].is_set())
	{
		if(isFile(args["colors"]))
		{
			string filename = args["colors"];
			ifstream file(filename.c_str());
			vector<ColorType> tmp;
			file>>tmp;
			colors.insert(tmp.begin(),tmp.end());
		}
		else
		{
			vector<ColorType> tmp = toVector<ColorType>(args["colors"]);
			colors.insert(tmp.begin(),tmp.end());
		}
	}
	
	// We convert the image
	ColorMap* colorMap = getColorMapFromFile(args["fitsFile"]);
	
	// We fill holes if requested
	if(args["fill"])
	{
		colorMap->removeHoles();

		#if defined DEBUG
		colorMap->writeFits(filenamePrefix + "filled.fits");
		#endif
	}

	// We erase any colors that is not to be kept
	if(colors.size() > 0 && args["uniqueColor"].is_set())
	{
		ColorType uniqueColor = args["uniqueColor"];
		for(unsigned j = 0; j < colorMap->NumberPixels(); ++j)
		{
			if(colorMap->pixel(j) != colorMap->null())
			{
				if(colors.count(colorMap->pixel(j)) == 0)
				{
					colorMap->pixel(j) = colorMap->null();
				}
				else
				{
					colorMap->pixel(j) = uniqueColor;
				}
			}
		}
	}
	else if(args["uniqueColor"].is_set())
	{
		ColorType uniqueColor = args["uniqueColor"];
		for(unsigned j = 0; j < colorMap->NumberPixels(); ++j)
		{
			if(colorMap->pixel(j) != colorMap->null())
			{
				colorMap->pixel(j) = uniqueColor;
			}
		}
	}
	else if(colors.size() > 0)
	{
		for(unsigned j = 0; j < colorMap->NumberPixels(); ++j)
		{
			if(colorMap->pixel(j) != colorMap->null() && colors.count(colorMap->pixel(j)) == 0)
			{
				colorMap->pixel(j) = colorMap->null();
			}
		}
	}

	#if defined DEBUG
	if(colors.size() > 0 || args["uniqueColor"].is_set())
		colorMap->writeFits(filenamePrefix + "recolored.fits");
	#endif
	
	// We transform the image
	if(args["straightenUp"] || args["recenter"].is_set() || args["scaling"].is_set())
	{
		// We correct for the roll
		Real rotationAngle = 0;
		if (args["straightenUp"])
		{
			rotationAngle = - colorMap->Crota2();
		}
	
		// We recenter the image
		RealPixLoc newCenter = colorMap->SunCenter();
		if(args["recenter"].is_set() && !readCoordinate(newCenter, args["recenter"]))
		{
			cerr<<"Error : Cannot convert "<<args["recenter"]<<" to coordinates"<<endl;
			return EXIT_FAILURE;
		}
	
		// We scale the image
		Real scaling = 1.;
		if(args["scaling"].is_set())
			scaling = args["scaling"];
	
		colorMap->transform(rotationAngle, RealPixLoc(newCenter.x - colorMap->SunCenter().x, newCenter.y - colorMap->SunCenter().y), scaling);
	
		#if defined DEBUG
		colorMap->writeFits(filenamePrefix + "transformed.fits");
		#endif
	}
	
	// We make the png
	MagickImage outputImage = colorMap->magick(backgroundColor);
	
	// We label the image
	if(args["upperLabel"].is_set())
	{
		string text = args["upperLabel"];
		if(text.empty())
			text = colorMap->Label();
		else
			text = colorMap->getHeader().expand(text);
		
		size_t text_size = colorMap->Xaxes()/40;
		outputImage.fillColor("white");
		outputImage.font(FONT);
		outputImage.fontPointsize(text_size);
		outputImage.annotate(text, Geometry(0, 0, text_size/2, text_size/2), Magick::NorthWestGravity);
	}
	
	// We label the image
	if(args["lowerLabel"].is_set())
	{
		string text = colorMap->getHeader().expand(args["lowerLabel"]);
		size_t text_size = colorMap->Xaxes()/40;
		outputImage.fillColor("white");
		outputImage.font(FONT);
		outputImage.fontPointsize(text_size);
		outputImage.annotate(text, Geometry(0, 0, text_size/2, text_size/2), Magick::SouthWestGravity);
	}
	
	delete colorMap;
	
	// We resize the image
	outputImage.scale(size_geometry);
	
	// We write down the image
	outputImage.write(filenamePrefix + "png");
	
	return EXIT_SUCCESS;
}
