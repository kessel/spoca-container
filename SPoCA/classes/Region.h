#pragma once
#ifndef Region_H
#define Region_H

#include <limits>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <string>
#include <set>

#include "constants.h"
#include "tools.h"
#include "Coordinate.h"
#include "ColorMap.h"
#include "FitsFile.h"

//! Class to obtain information about the position in space and time of a region

class Region
{
	protected :
		//! Unique and invariable identifier for a region at time observationTime
		unsigned id;
		
		//! Observation Time for the region
		time_t observationTime;
		
		//! Corresponding color of the pixels of the region in a ColorMap
		ColorType color;
		
		//! Coordonates of the first (lower left) pixel of the region
		PixLoc first;
		
		//! Coordonates of the lower left corner of the box surrounding the region
		PixLoc boxmin;
		
		//! Coordonates of the upper right corner of the box surrounding the region
		PixLoc boxmax;
		
		//! Time of first observation of the region
		time_t firstObservationTime;
		
		//! Total number of pixels in the region
		unsigned numberPixels;
		
		//! Coordinates of the center of the region
		RealPixLoc center;
		
		//! Accumulators to compute the area and the errors
		Real centerxError, centeryError, areaProjected, areaProjectedUncertainity, areaDeprojected, areaDeprojectedUncertainity;
		
	public :
		//! Constructor
		Region(const unsigned id = 0);
		//! Constructor
		Region(const time_t& observationTime);
		//! Constructor
		Region(const time_t& observationTime, const unsigned id, const ColorType color = 0);
		
		//! Comparison operator
		bool operator==(const Region& r)const;
		
		//! Accessor to retrieve the id
		unsigned Id() const;
		
		//! Accessor to set the id
		void setId(const unsigned& id);
		
		//! Accessor to retrieve the color
		ColorType Color() const;
		
		//! Accessor to set the color
		void setColor(const ColorType& color);
		
		//! Routine that returns the lower left corner coordinate of the box surrounding the region
		PixLoc Boxmin() const;
		
		//! Routine that returns the upper right corner coordinate of the box surrounding the region
		PixLoc Boxmax() const;
		
		//! Routine that returns the very first pixel of the region.
		PixLoc FirstPixel() const;
		
		//! Accessor to retrieve the observation time
		time_t ObservationTime() const;
		
		//! Accessor to retrieve the first observation time
		time_t FirstObservationTime() const;
		
		//! Accessor to set the first observation time
		void setFirstObservationTime(const time_t& t);
		
		//! Accessor to retrieve the observation time as a string
		std::string ObservationDate() const;
		
		//! Accessor to retrieve the first observation time as a string
		std::string FirstObservationDate() const;
		
		//! Accessor to retrieve the number of pixels
		unsigned NumberPixels() const;
		
		//! The geometric center of the region
		RealPixLoc Center() const;
		
		//! Error for the center x coordinate in pixels
		Real CenterxError() const;
		
		//! Error for the center y coordinate in pixels
		Real CenteryError() const;
		
		//! Area of the region as seen by the observer (arcsec²)
		Real Area_Projected() const;
		
		//! Uncertainty of the Area Projected
		Real Area_Projected_Uncertainity() const;
		
		//! Area of the region on the solar sphere (Mm²)
		Real Area_Deprojected() const;
		
		//! Uncertainty of the Area Deprojected
		Real Area_Deprojected_Uncertainity() const;
		
		//! Routine that returns a label for the HEK.
		std::string HekLabel() const;
		
		//! Output a region as a string
		std::string toString(const std::string& separator, bool header = false) const;
		
		//! Routine to update a region with a new pixel coordinate
		void add(const PixLoc& coordinate, const bool& atBorder, const RealPixLoc& sunCenter, const Real& sun_radius, const Real pixelLength, const Real pixelWidth);
		
		//! Routine that generate a chaincode for the connected component indicated by firstPixel
		std::vector<PixLoc> chainCode(const ColorMap* image, const unsigned min_points, const unsigned max_points, Real max_deviation = 0.) const;

	public :
		friend FitsFile& readRegions(FitsFile& file, std::vector<Region*>& regions, bool getTrackedColors);

};

//! Extraction of the regions from a ColorMap
/*
@param map A map of the region, each one must have a different color
*/
std::vector<Region*> getRegions(const ColorMap* coloredMap);

//! Extraction of the regions from a ColorMap
/*
@param map A map of the region, each one must have a different color
@param colors The regions color for which to compute the stats
*/
std::vector<Region*> getRegions(const ColorMap* coloredMap, const std::set<ColorType>& colors);

//! Write the regions into a fits file as column into the current table
FitsFile& writeRegions(FitsFile& file, const std::vector<Region*>& regions);

//! Read the regions from the fits file current table
FitsFile& readRegions(FitsFile& file, std::vector<Region*>& regions, bool getTrackedColors = false);

#endif
