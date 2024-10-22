#pragma once
#ifndef STAFFStats_H
#define STAFFStats_H

#include <limits>
#include <cmath>
#include <deque>

#include "constants.h"
#include "tools.h"
#include "Coordinate.h"
#include "EUVImage.h"
#include "ColorMap.h"
#include "FitsFile.h"
#include "Region.h"

//! Class to obtain information about the statistics of a region for the STAFF
/*!
This class gives statistics information about a Region such as center, center error, area,
and common statistics mesure on the intensities of the region like mean, variance, etc.

*/

class STAFFStats
{

	private :
		//! Unique and invariable identifier for a region at time observationTime
		unsigned id;
		
		//! Observation Time for the stats
		time_t observationTime;
		//! Total number of pixels in the class
		unsigned numberPixels;
		// Moments
		mutable Real m2, m3, m4;
		Real minIntensity, maxIntensity, totalIntensity, area_Raw, area_AtDiskCenter, fillingFactor;
		mutable std::deque<EUVPixelType> intensities;

	private :
		//! Routine to compute the moments from the pixel intensities vector
		void computeMoments();

	public :
		//! Constructor
		STAFFStats(const time_t& observationTime, const unsigned id = 0);
		
		//! Accessor to retrieve the id
		unsigned  Id() const;
		
		//! Accessor to set the id
		void setId(const unsigned& id);
		
		//! Accessor to retrieve the observation time
		time_t ObservationTime() const;
		
		//! Accessor to retrieve the observation time as a string
		std::string ObservationDate() const;
		
		//! Accessor to retrieve the number of pixels
		unsigned NumberPixels() const;
		//! Mean of the intensities the region
		Real Mean() const;
		//! Median of the intensities the region
		Real Median() const;
		//! Variance of the intensities the region
		Real Variance() const;
		//! Low quartile (25 percentile) of the intensities the class
		Real LowerQuartile() const;
		//! High quartile (75 percentile) of the intensities the class
		Real UpperQuartile() const;
		//! Skewness of the intensities the region
		Real Skewness() const;
		//! Kurtosis of the intensities the region
		Real Kurtosis() const;
		//! Minimum intensity of the region
		Real MinIntensity() const;
		//! Maximum intensity of the region
		Real MaxIntensity() const;
		//! Total intensity of the region
		Real TotalIntensity() const;
		//! Area of the class as seen on the image (Mm²)
		Real Area_Raw() const;
		//! Area of the class as it would be if the class was centered on the disk (Mm²)
		Real Area_AtDiskCenter() const;
		//! Filling factor of the class
		Real FillingFactor() const;
		
		//! Output a region as a string
		std::string toString(const std::string& separator, bool header = false) const;

		//! Routine to update a class with a new pixel
		void add(const PixLoc& coordinate, const EUVPixelType& pixelIntensity, const RealPixLoc& sunCenter, const Real& sun_radius);
		
		// We must make the getSTAFFStats functions as friends so they can correct the filling factor
		friend STAFFStats getSTAFFStats(const ColorMap*, ColorType, const EUVImage*);
		friend std::vector<STAFFStats> getSTAFFStats(const ColorMap*, ColorType, const ColorMap*, ColorType, const EUVImage*);
};


//! Compute STAFF statistics of an image using a ColorMap as a cache
/* 
@param coloredMap A segmented map
@param color The color for wich to extract the stats
@param image The image to compute the intensities statistics.
*/
STAFFStats getSTAFFStats(const ColorMap* coloredMap, ColorType color, const EUVImage* image);

//! Compute STAFF statistics of an image using 2 ColorMaps as caches
/* 
@param CHMap A segmented map for the CH
@param CHClass The color for wich to extract the stats on the CHMap
@param ARMap A segmented map for the AR
@param ARClass The color for wich to extract the stats on the ARMap
@param image The image to compute the intensities statistics.
*/
std::vector<STAFFStats> getSTAFFStats(const ColorMap* CHMap, ColorType CHClass, const ColorMap* ARMap, ColorType ARClass, const EUVImage* image);

//! Write the regions into a fits file as column into the current table 
FitsFile& writeRegions(FitsFile& file, const std::vector<STAFFStats>& regions_stats);

#endif
