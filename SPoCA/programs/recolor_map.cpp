//! This program recolors a map fits file. You must provide one and only one of a lookup color table, a list of colors to erase or a list of colors to keep.

/*!
<BR>Version: 3.0
<BR>Author: Benjamin Mampaey, benjamin.mampaey@sidc.be
<BR>Compiled on Oct 29 2014 with options :
<BR>NUMBERCHANNELS: 2
<BR>EUVPixelType: f
<BR>Real: f

@section usage Usage
<tt> bin/recolor_map.x [-option optionvalue ...]  fitsFile [ fitsFile ... ] </tt>

@param fitsFile	Path to the map fits file to be recolored

global parameters:

@param help	Print a help message and exit.
<BR>If you pass the value doxygen, the help message will follow the doxygen convention.
<BR>If you pass the value config, the help message will write a configuration file template.

@param config	Program option configuration file.

@param color_lookup_table	A file containing a color lookup table

@param erase_colors	The list of color to erase separated by commas or a file containg such a list.

@param keep_colors	The list of color to keep separated by commas or a file containg such a list. All other colors will be erased

@param output	The path of the the output directory.


@page recolor_map recolor_map.x

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
#include "../classes/FitsFile.h"

using namespace std;


string filenamePrefix;

int main(int argc, const char **argv)
{
	// We declare our program description
	string programDescription = "This program recolors a map fits file. You must provide one and only one of a lookup color table, a list of colors to erase or a list of colors to keep.";
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
	
	args["color_lookup_table"] = ArgParser::Parameter("", 'c', "A file containing a color lookup table");
	args["erase_colors"] = ArgParser::Parameter("", 'e', "The list of color to erase separated by commas or a file containg such a list.");
	args["keep_colors"] = ArgParser::Parameter("", 'k', "The list of color to keep separated by commas or a file containg such a list. All other colors will be erased");

	args["output"] = ArgParser::Parameter(".", 'O', "The path of the the output directory.");
	args["fitsFile"] = ArgParser::RemainingPositionalParameters("Path to the map fits file to be recolored", 1);
	
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
	
	if(!isDir(args["output"]))
	{
		cerr<<"Error : "<<args["output"]<<" is not a directory!"<<endl;
		return EXIT_FAILURE;
	}

	// Check that the user selected one and only one recoloring option
	if((args["color_lookup_table"].is_set()?1:0) + (args["erase_colors"].is_set()?1:0) + (args["keep_colors"].is_set()?1:0) != 1)
	{
		cerr<<"Error : please select one and only one recoloration method."<<endl;
		return EXIT_FAILURE;
	}
	
	// We parse the color lookup table
	std::map<ColorType,ColorType> color_lookup_table;
	if(args["color_lookup_table"].is_set())
	{
		if(isFile(args["color_lookup_table"]))
		{
			string filename = args["color_lookup_table"];
			ifstream file(filename.c_str());
			ColorType from, to;
			while(file.good())
			{
				while(file.good() && !isdigit(char(file.peek())))
				{
					file.get();
				}
				if (file.good())
					file>>from;
				while(file.good() && !isdigit(char(file.peek())))
				{
					file.get();
				}
				if (file.good())
					file>>to;
				if (file.good())
					color_lookup_table[from] = to;
			}
		}
		else
		{
			cerr<<"Error : "<<args["color_lookup_table"]<<" is not a file!"<<endl;
			return EXIT_FAILURE;
		}
	}

	// We parse the colors of the regions to erase
	set<ColorType> erase_colors;
	if(args["erase_colors"].is_set())
	{
		if(isFile(args["erase_colors"]))
		{
			string filename = args["erase_colors"];
			ifstream file(filename.c_str());
			vector<ColorType> tmp;
			file>>tmp;
			erase_colors.insert(tmp.begin(),tmp.end());
		}
		else
		{
			vector<ColorType> tmp = toVector<ColorType>(args["erase_colors"]);
			erase_colors.insert(tmp.begin(),tmp.end());
		}
	}
	// We parse the colors of the regions to keep
	set<ColorType> keep_colors;
	if(args["keep_colors"].is_set())
	{
		if(isFile(args["keep_colors"]))
		{
			string filename = args["keep_colors"];
			ifstream file(filename.c_str());
			vector<ColorType> tmp;
			file>>tmp;
			keep_colors.insert(tmp.begin(),tmp.end());
		}
		else
		{
			vector<ColorType> tmp = toVector<ColorType>(args["keep_colors"]);
			keep_colors.insert(tmp.begin(),tmp.end());
		}
	}
	
	// We recolor the images
	deque<string> imagesFilenames = args.RemainingPositionalArguments();
	for (unsigned p = 0; p < imagesFilenames.size(); ++p)
	{
		ColorMap* colorMap = getColorMapFromFile(imagesFilenames[p]);
		// We recolor the map
		if(! color_lookup_table.empty())
		{
			colorMap->recolorizeConnectedComponents(color_lookup_table);
		}
		else if(! erase_colors.empty())
		{
			colorMap->eraseColors(erase_colors);
		}
		else if(! keep_colors.empty())
		{
			colorMap->keepColors(keep_colors);
		}

		colorMap->writeFits(makePath(args["output"], stripPath(imagesFilenames[p])), FitsFile::compress);
		delete colorMap;
	}
	
	return EXIT_SUCCESS;
}
