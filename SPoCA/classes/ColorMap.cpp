#include "ColorMap.h"
#include <assert.h>
#include <deque>
#include <math.h>
#include <algorithm>

extern std::string filenamePrefix;

using namespace std;

ColorMap::~ColorMap()
{}

ColorMap::ColorMap(const unsigned& xAxes, const unsigned& yAxes)
:SunImage<ColorType>(xAxes, yAxes)
{
	nullpixelvalue = 0;

}

ColorMap::ColorMap(const Header& header, const unsigned& xAxes, const unsigned& yAxes)
:SunImage<ColorType>(header, xAxes, yAxes)
{
	nullpixelvalue = 0;
}

ColorMap::ColorMap(const WCS& wcs, const unsigned& xAxes, const unsigned& yAxes)
:SunImage<ColorType>(wcs, xAxes, yAxes)
{
	nullpixelvalue = 0;
}

ColorMap::ColorMap(const SunImage<ColorType>& i)
:SunImage<ColorType>(i)
{
	nullpixelvalue = 0;

}

ColorMap::ColorMap(const SunImage<ColorType>* i)
:SunImage<ColorType>(i)
{
	nullpixelvalue = 0;
}

inline ColorType ColorMap::interpolate(float x, float y) const
{
	x = x < 0.? 0. : min(float(xAxes-1.001), x);
	y = y < 0.? 0. : min(float(yAxes-1.001), y);
	
	unsigned ix = (unsigned) x;
	unsigned iy = (unsigned) y;
	float dx = x - ix;
	float dy = y - iy;
	float cdx = 1. - dx;
	float cdy = 1. - dy;
	ColorType colors[] = {pixel(ix, iy), pixel(ix+1, iy), pixel(ix, iy+1), pixel(ix+1, iy+1)};
	float quantity[] = {cdx*cdy, dx*cdy, cdx*dy, dx*dy};
	
	for(unsigned i = 0; i < 3; ++i)
		for(unsigned j = i+1; j < 4; ++j)
			if(colors[i] == colors[j])
			{
				quantity[i] += quantity[j];
				quantity[j] = 0;
			}
	unsigned max = 0;
	for(unsigned i = 1; i < 4; ++i)
		max = quantity[i] > quantity[max] ? i : max;
	
	return colors[max];
}

inline ColorType ColorMap::interpolate(const RealPixLoc& c) const
{
	return interpolate(c.x, c.y);
}

void ColorMap::parseHeader()
{
	// We parse the header to extract the wcs coordinate system
	if(header.has("CRPIX1") and header.has("CRPIX2"))
		wcs.setSunCenter(header.get<Real>("CRPIX1") - 1, header.get<Real>("CRPIX2") - 1);
	else
		cerr<<"Error: Fits header not conform: No CRPIX1 or CRPIX2 keyword."<<endl;
	
	if (header.has("T_OBS"))
		wcs.setDateObs(header.get<string>("T_OBS"));
	else if (header.has("DATE_OBS"))
		wcs.setDateObs(header.get<string>("DATE_OBS"));
	else if (header.has("DATE-OBS"))
		wcs.setDateObs(header.get<string>("DATE-OBS"));
	
	if(header.has("CTYPE1") and header.has("CTYPE2"))
		wcs.setCType(header.get<string>("CTYPE1"), header.get<string>("CTYPE2"));
	else
		cerr<<"Error: Fits header not conform: No CTYPE1 or CTYPE2 keyword."<<endl;
	
	if(header.has("CDELT1") and header.has("CDELT2"))
		wcs.setCDelt(header.get<Real>("CDELT1"), header.get<Real>("CDELT2"));
	else
		cerr<<"Error: Fits header not conform: No CDELT1 or CDELT2 keyword."<<endl;
	
	if (header.has("HGLT_OBS"))
		wcs.setB0(header.get<Real>("HGLT_OBS"));
	else if (header.has("CRLT_OBS"))
		wcs.setB0(header.get<Real>("CRLT_OBS"));
	else if(header.has("SOLAR_B0"))
		wcs.setB0(header.get<Real>("SOLAR_B0"));
	else if (wcs.time_obs != 0)
		wcs.setB0(earth_latitude(wcs.time_obs));
	
	if (header.has("HGLN_OBS"))
		wcs.setL0(header.get<Real>("HGLN_OBS"));
	else
		wcs.setL0(0);
	
	if (header.has("CRLN_OBS"))
		wcs.setCarringtonL0(header.get<Real>("CRLN_OBS"));
	
	if (header.has("DSUN_OBS"))
		wcs.setDistanceSunObs(header.get<double>("DSUN_OBS")/1000000.);
	else if (wcs.time_obs != 0)
		wcs.setDistanceSunObs(distance_sun_earth(wcs.time_obs));
	
	if (header.has("CD1_1") and header.has("CD1_2") and header.has("CD2_1") and header.has("CD2_2"))
	{
		wcs.setCD(header.get<Real>("CD1_1"), header.get<Real>("CD1_2"), header.get<Real>("CD2_1"), header.get<Real>("CD2_2"));
	}
	else if (header.has("PC1_1") and header.has("PC1_2") and header.has("PC2_1") and header.has("PC2_2"))
	{
		wcs.setPC(header.get<Real>("PC1_1"), header.get<Real>("PC1_2"), header.get<Real>("PC2_1"), header.get<Real>("PC2_2"));
	}
	else if (header.has("CROTA2"))
	{
		wcs.setCrota2(header.get<Real>("CROTA2"));
	}
	
	// We read the radius
	if(header.has("RSUN_OBS"))
	{
		wcs.setSunradius(header.get<Real>("RSUN_OBS")/wcs.cdelt1);
	}
	else if(header.has("R_SUN"))
	{
		wcs.setSunradius(header.get<Real>("R_SUN"));
	}
	else
	{
		cerr<<"Error: No sun radius found in header"<<endl;
		exit(EXIT_FAILURE);
	}
	
	if(header.has("RSUN_REF"))
	{
		wcs.setSunradiusMm(header.get<Real>("RSUN_REF")/1000000.);
	}
}

void ColorMap::fillHeader()
{
	header.set<Real>("CRPIX1", wcs.sun_center.x + 1);
	header.set<Real>("CRPIX2", wcs.sun_center.y + 1);
	header.set<string>("CTYPE1", wcs.ctype1);
	header.set<string>("CTYPE2", wcs.ctype2);
	header.set<Real>("CDELT1", wcs.cdelt1);
	header.set<Real>("CDELT2", wcs.cdelt2);
	header.set<string>("CUNIT1", wcs.cunit1);
	header.set<string>("CUNIT2", wcs.cunit2);
	header.set<Real>("HGLT_OBS", wcs.b0 * RADIAN2DEGREE, "[deg]");
	header.set<Real>("HGLN_OBS", wcs.l0 * RADIAN2DEGREE, "[deg]");
	header.set<Real>("CRLN_OBS", wcs.carrington_l0 * RADIAN2DEGREE, "[deg]");
	header.set<Real>("DSUN_OBS", wcs.dsun_obs * 1000000., "[m]");
	header.set<Real>("CD1_1", wcs.cd[0][0]);
	header.set<Real>("CD1_2", wcs.cd[0][1]);
	header.set<Real>("CD2_1", wcs.cd[1][0]);
	header.set<Real>("CD2_2", wcs.cd[1][1]);
	header.set<string>("DATE_OBS",  wcs.date_obs);
	header.set<Real>("RSUN_OBS", wcs.sun_radius * wcs.cdelt1, "[arcsec]");
	header.set<Real>("RSUN_REF", wcs.sunradius_Mm * 1000000., "[m]");
}


bool isColorMap(const Header& header)
{
	return header.has("INSTRUME") && header.get<string>("INSTRUME").find("SPoCA") != string::npos;
}



void ColorMap::thresholdRegionsByRawArea(const double minSize)
{
	const double pixelarea = PixelArea();
	
	//First we compute the area for each color
	map<ColorType,double> areas;
	for (ColorType* j = pixels; j < pixels + numberPixels; ++j)
	{
		if(*j != nullpixelvalue)
		{
			if (areas.count(*j) == 0)
				areas[*j] = pixelarea;
			else
				areas[*j] += pixelarea;
		}
	}
	#if defined VERBOSE
	cout<<"Found following sizes for regions:"<<endl;
	for(map<ColorType,double>::iterator a = areas.begin(); a != areas.end(); ++a)
	{
		cout<<"color: "<<a->first<<"\tsize:"<<a->second<<(a->second < minSize ? " To small" : " OK")<<endl;
	}
	#endif
	//Now we nullify those that are too small
	for (ColorType* j = pixels; j < pixels + numberPixels; ++j)
	{
		if(*j != nullpixelvalue && areas[*j] < minSize)
		{
			*j = nullpixelvalue;
		}
	}

}

void ColorMap::thresholdRegionsByRealArea(double minSize)
{
	//minSize is given as a number of pixels so we convert to Mm2
	minSize *= RealPixelArea(wcs.sun_center);
	
	//We compute the area for each color
	map<ColorType,Real> areas;
	
	for (PixLoc j(0,0); j.y < yAxes; ++j.y)
	{
		for (j.x = 0; j.x < xAxes; ++j.x)
		{
			const ColorType& color = pixel(j);
			if(color != nullpixelvalue)
			{
				if (areas.count(color) == 0)
					areas[color] = RealPixelArea(j);
				else
					areas[color] += RealPixelArea(j);
			}
		}
	}
	
	#if defined VERBOSE
	cout<<"Found following sizes for regions:"<<endl;
	for(map<ColorType,Real>::iterator a = areas.begin(); a != areas.end(); ++a)
	{
		cout<<"color: "<<a->first<<"\tsize:"<<a->second<<(a->second < minSize ? " To small" : " OK")<<endl;
	}
	#endif
	
	//Now we nullify those that are too small
	for (ColorType* j = pixels; j < pixels + numberPixels; ++j)
	{
		if(*j != nullpixelvalue && areas[*j] < minSize)
		{
			*j = nullpixelvalue;
		}
	}

}


ColorMap* ColorMap::dilateDiamond(unsigned size, ColorType pixelValueToDilate)
{

	unsigned *manthanDistance = new unsigned[xAxes * yAxes];
	unsigned maxDistance = xAxes + yAxes;

	for (unsigned y=0; y < yAxes; ++y)
	{
		for (unsigned x=0; x < xAxes; ++x)
		{
			if (pixel(x,y) == pixelValueToDilate)
			{

				manthanDistance[x+y*xAxes] = 0;
			}
			else
			{

				manthanDistance[x+y*xAxes] = maxDistance;

				if (x>0) manthanDistance[x+y*xAxes] = manthanDistance[x+y*xAxes] < (manthanDistance[x-1+y*xAxes]+1) ? manthanDistance[x+y*xAxes] : (manthanDistance[x-1+y*xAxes]+1);

				if (y>0) manthanDistance[x+y*xAxes] = manthanDistance[x+y*xAxes] < (manthanDistance[x+(y-1)*xAxes]+1) ? manthanDistance[x+y*xAxes] : (manthanDistance[x+(y-1)*xAxes]+1);
			}
		}
	}

	for (unsigned y=yAxes; y >0; )
	{
		--y;
		for (unsigned x=xAxes; x >0; )
		{

			--x;
			if (x+1<xAxes) manthanDistance[x+y*xAxes] = manthanDistance[x+y*xAxes] < (manthanDistance[x+1+y*xAxes]+1) ? manthanDistance[x+y*xAxes] : (manthanDistance[x+1+y*xAxes]+1);

			if (y+1<yAxes) manthanDistance[x+y*xAxes] = manthanDistance[x+y*xAxes] < (manthanDistance[x+(y+1)*xAxes]+1) ? manthanDistance[x+y*xAxes] : (manthanDistance[x+(y+1)*xAxes]+1);

		}
	}

	for (unsigned y=0; y < yAxes; ++y)
		for (unsigned x=0; x < xAxes; ++x)
			if(manthanDistance[x+y*xAxes] <= size) pixel(x,y) = pixelValueToDilate;

	delete[] manthanDistance;
	return this;

}



ColorMap* ColorMap::erodeDiamond(unsigned size, ColorType pixelValueToErode)
{

	ColorType fillPixelValue = nullpixelvalue;
	unsigned *manthanDistance = new unsigned[xAxes * yAxes];
	unsigned maxDistance = xAxes + yAxes;

	for (unsigned y=0; y < yAxes; ++y)
	{
		for (unsigned x=0; x < xAxes; ++x)
		{
			if (pixel(x,y) != pixelValueToErode)
			{

				manthanDistance[x+y*xAxes] = 0;
			}
			else
			{

				manthanDistance[x+y*xAxes] = maxDistance;

				if (x>0) manthanDistance[x+y*xAxes] = manthanDistance[x+y*xAxes] < (manthanDistance[x-1+y*xAxes]+1) ? manthanDistance[x+y*xAxes] : (manthanDistance[x-1+y*xAxes]+1);

				if (y>0) manthanDistance[x+y*xAxes] = manthanDistance[x+y*xAxes] < (manthanDistance[x+(y-1)*xAxes]+1) ? manthanDistance[x+y*xAxes] : (manthanDistance[x+(y-1)*xAxes]+1);
			}
		}
	}

	for (unsigned y=yAxes; y >0; )
	{
		--y;
		for (unsigned x=xAxes; x >0;)
		{
			--x;
			if (x+1<xAxes) manthanDistance[x+y*xAxes] = manthanDistance[x+y*xAxes] < (manthanDistance[x+1+y*xAxes]+1) ? manthanDistance[x+y*xAxes] : (manthanDistance[x+1+y*xAxes]+1);

			if (y+1<xAxes) manthanDistance[x+y*xAxes] = manthanDistance[x+y*xAxes] < (manthanDistance[x+(y+1)*xAxes]+1) ? manthanDistance[x+y*xAxes] : (manthanDistance[x+(y+1)*xAxes]+1);

		}
	}

	for (unsigned y=0; y < yAxes; ++y)
		for (unsigned x=0; x < xAxes; ++x)
			pixel(x,y) = manthanDistance[x+y*xAxes] <= size? fillPixelValue : pixelValueToErode;

	delete[] manthanDistance;
	return this;

}



ColorMap* ColorMap::dilateCircular(const Real size, const ColorType unsetValue)
{
	ColorType * newPixels = new ColorType[numberPixels];
	memcpy(newPixels, pixels, numberPixels * sizeof(ColorType));
	vector<int> shape;
	shape.reserve(unsigned(size*size*3));
	for(unsigned x = 1; x <= size; ++x)
		shape.push_back(x);
	for(int x = -int(size); x <= int(size); ++x)
		for(unsigned y = 1; y <= size; ++y)
			if(sqrt(x * x + y * y) <= size)
				shape.push_back(y * xAxes + x);
	
	int offset = xAxes + 1;
	ColorType * j = pixels + offset;
	ColorType * nj = newPixels + offset;
	for(unsigned y = 1; y < yAxes - 1; ++y)
	{
		for(unsigned x = 1; x < xAxes - 1; ++x)
		{
			if(*j != unsetValue)
			{
				if(*(j-1) == unsetValue || *(j+1) == unsetValue || *(j-xAxes) == unsetValue || *(j+xAxes) == unsetValue)
				{
					for(unsigned s = 0; s < shape.size(); ++s)
					{
						if(offset + shape[s] < int(numberPixels))
							*(nj + shape[s]) = *j;
						if(shape[s] <= offset)
							*(nj - shape[s]) = *j;
					}
				}
			}
			++j;
			++nj;
			++offset;
		}
		j+=2;
		nj+=2;
		offset+=2;
	}
	
	delete[] pixels;
	pixels = newPixels;
	return this;
}



ColorMap* ColorMap::erodeCircular(const Real size, const ColorType unsetValue)
{
	ColorType * newPixels = new ColorType[numberPixels];
	memcpy(newPixels, pixels, numberPixels * sizeof(ColorType));
	vector<int> shape;
	shape.reserve(unsigned(size*size*3));
	for(unsigned x = 1; x <= size; ++x)
		shape.push_back(x);
	for(int x = -int(size); x <= int(size); ++x)
		for(unsigned y = 1; y <= size; ++y)
			if(sqrt(x * x + y *y) <= size)
				shape.push_back(y * xAxes + x);
	
	int offset = xAxes + 1;
	ColorType * j = pixels + offset;
	ColorType * nj = newPixels + offset;
	for(unsigned y = 1; y < yAxes - 1; ++y)
	{
		for(unsigned x = 1; x < xAxes - 1; ++x)
		{
			if(*j != unsetValue && (*(j-1) != *j || *(j+1) != *j || *(j-xAxes) != *j || *(j+xAxes) != *j))
			{
				*nj = unsetValue;
				for(unsigned s = 0; s < shape.size(); ++s)
				{
					if(offset + shape[s] < int(numberPixels))
						*(nj + shape[s]) = unsetValue;
					if(shape[s] <= offset)
						*(nj - shape[s]) = unsetValue;
				}
				
			}
			++offset;
			++j;
			++nj;
		}
		j+=2;
		nj+=2;
		offset+=2;
	}
	
	delete[] pixels;
	pixels = newPixels;
	return this;
}

vector<HCC> ColorMap::get_half_circle(Real size)
{
	vector<HCC> line;
	RealPixLoc sun_center = SunCenter();
	for (Real y = - size; y <= size; ++y)
	{
		Real x = sqrt(size * size - y * y);
		line.push_back(toHCC(RealPixLoc(sun_center.x + x, sun_center.y + y)));
	}
	return line;
}

vector<PixLoc> ColorMap::get_shape(PixLoc center, const vector<HCC>& line)
{
	vector<PixLoc> shape;
	HGS hgs = toHGS(center);
	
	if(! hgs)
	{
		return shape;
	}
	// The rotation matrix is such that we need to invert THE latitude
	double sin_lat = sin(-hgs.latitude);
	double cos_lat = cos(-hgs.latitude);
	double sin_long = sin(hgs.longitude);
	double cos_long = cos(hgs.longitude);
	
	// We compute the rotation of the shape
	for (unsigned i = 0; i < line.size(); ++i)
	{
		// We rotate around the x axis, x stays the same
		double y = line[i].y * cos_lat - line[i].z * sin_lat;
		double z = line[i].y * sin_lat + line[i].z * cos_lat;
		// We rotate around the y axis, y stays the same
		RealPixLoc max = toRealPixLoc(HCC(line[i].x * cos_long + sin_long * z, y, -line[i].x * sin_long + cos_long * z));
		RealPixLoc min = toRealPixLoc(HCC(-line[i].x * cos_long + sin_long * z, y, line[i].x * sin_long + cos_long * z));
		// We fill up the shape
		for (unsigned x = round(min.x); x <= round(max.x); ++x)
			shape.push_back(PixLoc(x, round(min.y)));
	}
	return shape;
}

ColorMap* ColorMap::dilateCircularProjected(const Real size, const ColorType unsetValue)
{
	ColorType* original = new ColorType[numberPixels];
	memcpy(original, pixels, numberPixels * sizeof(ColorType));
	vector<HCC> line = get_half_circle(size);
	
	// We need to temporarly disable b0
	Real cos_b0 = wcs.cos_b0;
	Real sin_b0 = wcs.sin_b0;
	wcs.cos_b0 = 1;
	wcs.sin_b0 = 0;
	RealPixLoc sun_center = SunCenter();
	Real sun_radius = SunRadius();
	
	unsigned min_y = ceil(sun_center.y - sun_radius);
	unsigned max_y = floor(sun_center.y + sun_radius);
	for(unsigned y = min_y; y <= max_y; ++y)
	{
		Real delta_x = sqrt(sun_radius * sun_radius - (y - sun_center.y) * (y - sun_center.y));
		unsigned min_x = ceil(sun_center.x - delta_x);
		unsigned max_x = floor(sun_center.x + delta_x);
		ColorType* j = original + y * xAxes + min_x;
		for(unsigned x = min_x; x <= max_x; ++x, ++j)
		{
			if(*j != unsetValue)
			{
				if(*(j-1) == unsetValue || *(j+1) == unsetValue || *(j-xAxes) == unsetValue || *(j+xAxes) == unsetValue)
				{
					vector<PixLoc> shape = get_shape(PixLoc(x, y), line);
					for (vector<PixLoc>::iterator s = shape.begin() ; s != shape.end(); ++s)
					{
						pixel(*s) = *j;
					}
				}
			}
		}
	}
	wcs.cos_b0 = cos_b0;
	wcs.sin_b0 = sin_b0;
	delete[] original;
	return this;
}

ColorMap* ColorMap::erodeCircularProjected(const Real size, const ColorType unsetValue)
{
	ColorType* original = new ColorType[numberPixels];
	memcpy(original, pixels, numberPixels * sizeof(ColorType));
	vector<HCC> line = get_half_circle(size);
	
	// We need to temporarly disable b0
	Real cos_b0 = wcs.cos_b0;
	Real sin_b0 = wcs.sin_b0;
	wcs.cos_b0 = 1;
	wcs.sin_b0 = 0;
	RealPixLoc sun_center = SunCenter();
	Real sun_radius = SunRadius();
	
	unsigned min_y = ceil(sun_center.y - sun_radius);
	unsigned max_y = floor(sun_center.y + sun_radius);
	for(unsigned y = min_y; y <= max_y; ++y)
	{
		Real delta_x = sqrt(sun_radius * sun_radius - (y - sun_center.y) * (y - sun_center.y));
		unsigned min_x = ceil(sun_center.x - delta_x);
		unsigned max_x = floor(sun_center.x + delta_x);
		ColorType* j = original + y * xAxes + min_x;
		for(unsigned x = min_x; x <= max_x; ++x, ++j)
		{
			if(*j == unsetValue && (*(j-1) != unsetValue || *(j+1) != unsetValue || *(j-xAxes) != unsetValue || *(j+xAxes) != unsetValue))
			{
				vector<PixLoc> shape = get_shape(PixLoc(x, y), line);
				for (vector<PixLoc>::iterator s = shape.begin() ; s != shape.end(); ++s)
				{
					pixel(*s) = unsetValue;
				}
			}
		}
	}
	wcs.cos_b0 = cos_b0;
	wcs.sin_b0 = sin_b0;
	delete[] original;
	return this;
}


ColorMap* ColorMap::drawInternContours(const unsigned width, const ColorType unsetValue)
{

	ColorMap* eroded = new ColorMap(this);
	eroded->erodeCircular(width, unsetValue);
	for (unsigned j = 0; j < numberPixels; ++j)
	{
		if(eroded->pixels[j] != eroded->nullpixelvalue)
			pixels[j] = unsetValue;
	}
	delete eroded;
	return this;

}


ColorMap* ColorMap::drawExternContours(const unsigned width, const ColorType unsetValue)
{

	ColorMap * copy = new ColorMap (this);
	this->dilateCircular(width, unsetValue);
	for (unsigned j = 0; j < numberPixels; ++j)
	{
		if(pixels[j] == copy->pixels[j])
			pixels[j] = unsetValue;
	}
	delete copy;
	return this;

}


ColorMap* ColorMap::drawContours(const unsigned width, const ColorType unsetValue)
{
	unsigned size = width/2;
	if (size <= 0)
		size = 1;
	
	ColorType * newPixels = new ColorType[numberPixels];
	memcpy(newPixels, pixels, numberPixels * sizeof(ColorType));
	vector<unsigned> shape;
	shape.reserve(size*size*3);
	for(unsigned x = 1; x <= size; ++x)
		shape.push_back(x);
	for(int x = -size; x <= int(size); ++x)
		for(unsigned y = 1; y <= size; ++y)
			if(sqrt(x * x + y *y) <= size)
				shape.push_back(y * xAxes + x);
	
	
	int j = 0;
	for(unsigned y = size; y < yAxes - size; ++y)
	{
		j = y * xAxes + size;
		for(unsigned x = size; x < xAxes - size; ++x)
		{
			ColorType maxColor = pixels[j-1];
			maxColor = pixels[j+1] > maxColor ? pixels[j+1] : maxColor;
			maxColor = pixels[j-xAxes] > maxColor ? pixels[j-xAxes] : maxColor;
			maxColor = pixels[j+xAxes] > maxColor ? pixels[j+xAxes] : maxColor;
			if(pixels[j] != maxColor)
			{
				newPixels[j] = maxColor;
				for(unsigned s = 0; s < shape.size(); ++s)
				{
					#if defined EXTRA_SAFE
						if(j + shape[s] >= numberPixels || j - shape[s] < 0)
						{
							cerr<<"Error : trying to access pixel out of image in drawContours"<<endl;
							exit(EXIT_FAILURE);
						}
					#endif
					newPixels[j + shape[s]] = newPixels[j - shape[s]] = maxColor;
				}
				
			}
			else
			{
				newPixels[j] = unsetValue;
			}
			++j;
		}
	}
	
	delete[] pixels;
	pixels = newPixels;
	return this;
}


unsigned ColorMap::colorizeConnectedComponents(const ColorType setValue)
{
	ColorType color = setValue;
	for (unsigned j = 0; j < numberPixels; ++j)
	{
		if(pixels[j] == setValue)
		{
			++color;
			propagateColor(color, j);
		}
	}

	return unsigned(color - setValue);

}


void ColorMap::recolorizeConnectedComponents(const map<ColorType,ColorType>& LUT)
{
	ColorType* end = pixels + numberPixels;
	for (ColorType* j = pixels; j < end; ++j)
	{
		try {
			*j = LUT.at(*j);
		}
		catch (const out_of_range& oor) {
		}
	}
}

void ColorMap::eraseColors(const set<ColorType>& colors)
{
	ColorType* end = pixels + numberPixels;
	for (ColorType* j = pixels; j < end; ++j)
	{
		if (colors.count(*j)!=0)
			*j = nullpixelvalue;
	}
}
		

void ColorMap::keepColors(const set<ColorType>& colors)
{
	ColorType* end = pixels + numberPixels;
	for (ColorType* j = pixels; j < end; ++j)
	{
		if (colors.count(*j)==0)
			*j = nullpixelvalue;
	}
}

unsigned ColorMap::propagateColor(const ColorType color, const PixLoc& firstPixel)
{
	return propagateColor(color, firstPixel.x + firstPixel.y * xAxes);
}



unsigned ColorMap::propagateColor(const ColorType color, const unsigned firstPixel)
{
	deque<unsigned> pixelList;
	ColorType setValue = pixels[firstPixel];
	unsigned h;
	unsigned numberColoredPixels = 0;

	pixelList.push_back(firstPixel);
	while ( ! pixelList.empty())
	{
		h = pixelList.back();
		pixelList.pop_back();
		if(pixels[h] != setValue)
			continue;
		pixels[h] = color;
		++numberColoredPixels;
		if(h+1 < numberPixels && pixels[h+1] == setValue)
			pixelList.push_back(h+1);
		if(h+xAxes < numberPixels && pixels[h+xAxes] == setValue)
			pixelList.push_back(h+xAxes);
		if(h >= 1 && pixels[h-1] == setValue)
			pixelList.push_back(h-1);
		if(h >= xAxes && pixels[h-xAxes] == setValue)
			pixelList.push_back(h-xAxes);

	}
	return numberColoredPixels;
}



unsigned ColorMap::thresholdConnectedComponents(const unsigned minSize, const ColorType setValue)
{
	deque<unsigned> treatedPixels;
	ColorType color = setValue + 1;
	for (unsigned j = 0; j < numberPixels; ++j)
	{
		if(pixels[j] == setValue)
		{
			if (propagateColor(color, j) < minSize)
				propagateColor(nullpixelvalue, j);
			else
			{
				++color;
				treatedPixels.push_back(j);
			}
		}
	}
	//We have to give back the original color
	for (unsigned t = 0; t < treatedPixels.size(); ++t)
	{
		propagateColor(setValue, treatedPixels[t]);
	}

	return unsigned(color - 1 - setValue);
}


ColorMap* ColorMap::removeHoles(ColorType unusedColor)
{
	propagateColor(unusedColor, 0);
	ColorType lastColor = nullpixelvalue;
	for (unsigned j = 0; j < numberPixels; ++j)
	{
		if(pixels[j] != nullpixelvalue)
		{
			lastColor = pixels[j];
		}
		else
		{
			pixels[j] = lastColor;
		}
	}
	propagateColor(nullpixelvalue, 0);
	return this;
}

void ColorMap::preprocessing(const string& preprocessingList)
{
	double maxRadius = INF, maxLong = INF, maxLat = INF;
	
	vector<string> preprocessingSteps = split(preprocessingList);
	for(unsigned s = 0; s < preprocessingSteps.size(); ++s)
	{
		vector<string> stepParameters = split(preprocessingSteps[s], '=');
		string stepType = trimWhites(stepParameters[0]);
		
		#if defined VERBOSE
		cout<<"Applying image preprocessing step "<<stepType;
		if(stepParameters.size() > 1)
			cout<<" with parameter "<<stepParameters[1]<<endl;
		else
			cout<<endl;
		#endif
		
		if(stepType == "NAR")
		{
			maxRadius = stepParameters.size() > 1 ? min(maxRadius, toDouble(stepParameters[1])) : min(maxRadius, 1.);
		}
		else if(stepType == "Long")
		{
			if(stepParameters.size() < 2)
			{
				cerr<<"Error: No value specified for max Longitude preprocessing"<<preprocessingSteps[s]<<endl;
				exit(EXIT_FAILURE);
			}
			else
			{
				maxLong = min(maxLong, toDouble(stepParameters[1]));
			}
		}
		else if(stepType == "Lat")
		{
			if(stepParameters.size() < 2)
			{
				cerr<<"Error: No value specified for max Latitude preprocessing"<<preprocessingSteps[s]<<endl;
				exit(EXIT_FAILURE);
			}
			else
			{
				maxLat = min(maxLat, toDouble(stepParameters[1]));
			}
		}
		else
		{
			cerr<<"Error: Unknown preprocessing step "<<preprocessingSteps[s]<<endl;
			exit(EXIT_FAILURE);
		}
	}
	if(maxRadius < INF)
		nullifyAboveRadius(maxRadius);
	if(maxLong < 180 || maxLat < 180)
		nullifyAboveLongLat(maxLong, maxLat);
}


void ColorMap::computeButterflyStats(vector<float>& totalNumberOfPixels, vector<float>& regionNumberOfPixels, vector<float>& correctedTotalNumberOfPixels, vector<float>& correctedRegionNumberOfPixels)
{
	totalNumberOfPixels.clear();
	totalNumberOfPixels.resize(182, 0);
	regionNumberOfPixels.clear();
	regionNumberOfPixels.resize(182, 0);
	correctedTotalNumberOfPixels.clear();
	correctedTotalNumberOfPixels.resize(182, 0);
	correctedRegionNumberOfPixels.clear();
	correctedRegionNumberOfPixels.resize(182, 0);
	
	RealPixLoc sun_center = SunCenter();
	Real sun_radius = SunRadius();
	Real radius_squared = sun_radius * sun_radius;
	
	// We try to avoid computing latitude for pixels that are out of the sun disc
	unsigned miny = sun_center.y - sun_radius - 1;
	unsigned maxy = sun_center.y + sun_radius + 2;
	unsigned minx = sun_center.x - sun_radius - 1;
	unsigned maxx = sun_center.x + sun_radius + 2;
	for (unsigned y = miny; y < maxy ; ++y)
	{
		for (unsigned x = minx; x < maxx; ++x)
		{
			// We compute sigma, the square distance of the pixel to the sun edge
			Real dx = fabs(x - sun_center.x);
			Real dy = fabs(y - sun_center.y);
			Real sigma = radius_squared - (dx * dx) - (dy * dy);
			
			if(sigma > 0)
			{
				HGS hgs = toHGS(RealPixLoc(x, y));
				if (!!hgs)
				{
					//cout<<"Latitude of coord "<<RealPixLoc(x, y)<< " : "<< int(hgs.latitude * RADIAN2DEGREE)<< "\n";
					/* Pixels are gathered by the closest inferior integral latitude.
					I.e the latitude -0.5 will go into the -1 latitude
					but the latitude 0.5 will go into the 0 latitude
					*/
					// We compute the correction factor to compensate for the projection
					Real correction_factor = sun_radius/sqrt(sigma);
					int latitude = floor(hgs.latitude * RADIAN2DEGREE) + 91;
					++totalNumberOfPixels[latitude];
					// If the area correction factor is more than some value (i.e. the pixel is near the limb) we don't count it
					if (correction_factor <= HIGGINS_FACTOR)
						correctedTotalNumberOfPixels[latitude] += correction_factor;
					if(this->pixel(x, y) != this->null())
					{
						++regionNumberOfPixels[latitude];
						// If the area correction factor is more than some value (i.e. the pixel is near the limb) we don't count it
						if (correction_factor <= HIGGINS_FACTOR)
							correctedRegionNumberOfPixels[latitude] += correction_factor;
					}
				}
			}
		}
	}
}

#ifdef MAGICK
using namespace MagickCore;
MagickImage ColorMap::magick(const Magick::Color background)
{
	MagickImage image(background, xAxes, yAxes);
	for (unsigned y = 0; y < yAxes; ++y)
	{
		for (unsigned x = 0; x < xAxes; ++x)
		{
			if(pixel(x, y) != nullpixelvalue)
			{
				image.pixelColor(x, yAxes - y - 1, Magick::Color(gradient[pixel(x, y) % gradientMax]));
			}
		}
	}
	return image;
}

MagickImage ColorMap::magick()
{
	return magick(Magick::Color(0,0,0,QuantumRange));
}
#endif
