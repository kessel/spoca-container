// Functions to help main programs

#pragma once
#ifndef MainUtilities_H
#define MainUtilities_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sys/stat.h>

#include "FeatureVector.h"
#include "EUVImage.h"
#include "EITImage.h"
#include "EUVIImage.h"
#include "AIAImage.h"
#include "SWAPImage.h"
#include "HMIImage.h"
#include "ColorMap.h"

extern std::string filenamePrefix;
extern const char * instruments[];
extern const char * limb_corrections[];

// Read the centers and the wavelength from a file
unsigned readCentersFromFile(std::vector<RealFeature>& B, RealFeature& wavelengths, const std::string& centersFileName);

// Read the binsize
bool readbinSize(RealFeature& binSize, const std::string& sbinSize);

// Tell if a file exists
bool fileExists(const std::string& filename);

// Read the images from fits files
EUVImage* getImageFromFile(const std::string imageType, const std::string sunImageFileName);
std::vector<EUVImage*> getImagesFromFiles(const std::string imageType, const std::vector<std::string>& sunImagesFileNames, bool align = false);
ColorMap* getImageFromFile(const std::string sunImageFileName);

// Read the eta from a file
unsigned readEtaFromFile(std::vector<Real>& eta, const std::string& etaFileName);

// Read the max limits from file
unsigned readMaxLimitsFromFile (std::vector<RealFeature>& maxLimits, const std::string& maxLimitsFileName);

#endif
