#include "AIAImage.h"
#include "colortables.h"

using namespace std;

AIAImage::~AIAImage()
{}


AIAImage::AIAImage()
:EUVImage()
{}

AIAImage::AIAImage(const Header& header, const unsigned& xAxes, const unsigned& yAxes)
:EUVImage(header, xAxes, yAxes)
{}

AIAImage::AIAImage(const WCS& wcs, const unsigned& xAxes, const unsigned& yAxes)
:EUVImage(wcs, xAxes, yAxes)
{}


AIAImage::AIAImage(const EUVImage& i)
:EUVImage(i)
{}


AIAImage::AIAImage(const EUVImage* i)
:EUVImage(i)
{}

vector<Real> AIAImage::getALCParameters()
{
	Real parameters[] = AIA_ALC_PARAMETERS;
	vector<Real> temp(parameters, parameters + (sizeof(parameters)/sizeof(parameters[0])));
	for(unsigned p = 0; p < temp.size(); ++p)
		temp[p] /= 100.;
	return temp;
}

void AIAImage::parseHeader()
{
	wavelength = header.get<Real>("WAVELNTH");
	
	if (header.has("EXPTIME"))
		exposureTime = header.get<Real>("EXPTIME");
	else
		exposureTime = 1;
	
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

void AIAImage::fillHeader()
{
	header.set<Real>("WAVELNTH", wavelength);
	header.set<Real>("EXPTIME", exposureTime);
	header.set<Real>("CRPIX1", wcs.sun_center.x + 1);
	header.set<Real>("CRPIX2", wcs.sun_center.y + 1);
	header.set<string>("CTYPE1", wcs.ctype1);
	header.set<string>("CTYPE2", wcs.ctype2);
	header.set<Real>("CDELT1", wcs.cdelt1);
	header.set<Real>("CDELT2", wcs.cdelt2);
	header.set<string>("CUNIT1", wcs.cunit1);
	header.set<string>("CUNIT2", wcs.cunit2);
	header.set<Real>("HGLT_OBS", wcs.b0 * RADIAN2DEGREE);
	header.set<Real>("HGLN_OBS", wcs.l0 * RADIAN2DEGREE);
	header.set<Real>("CRLN_OBS", wcs.carrington_l0 * RADIAN2DEGREE);
	header.set<Real>("DSUN_OBS", wcs.dsun_obs*1000000.);
	header.set<Real>("CD1_1", wcs.cd[0][0]);
	header.set<Real>("CD1_2", wcs.cd[0][1]);
	header.set<Real>("CD2_1", wcs.cd[1][0]);
	header.set<Real>("CD2_2", wcs.cd[1][1]);
	header.set<string>("T_OBS",  wcs.date_obs);
	header.set<Real>("R_SUN", wcs.sun_radius);
	header.set<Real>("RSUN_REF", wcs.sunradius_Mm * 1000000.);
	
	if (header.has("CROTA2"))
	{
		header.set("CROTA2", wcs.getCrota2());
	}
	if (header.has("SC_ROLL"))
	{
		header.set("SC_ROLL", wcs.getCrota2());
	}
}

string AIAImage::Instrument() const
{
	return "AIA";
}

void AIAImage::enhance_contrast()
{
	// The following enhancement formulas are taken from the SolarSoft preocedure aia_intscale.pro
	switch (int(wavelength))
	{
		// bytscl(  sqrt((image*(4.99803/exptime)>(   1.5/ 1.06 )<(   50/ 1.06 ))))
		case 94:
			mul(4.99803/exposureTime);
			threshold(1.5 / 1.06, 50.0 / 1.06);
			for(unsigned j = 0; j < NumberPixels(); ++j)
			{
				pixels[j] = sqrt(pixels[j]);
			}
		break;
		
		// bytscl(alog10((image*(6.99685/exptime)>(   7.0/ 1.49 )<( 1200/ 1.49 ))))
		case 131:
			mul(6.99685/exposureTime);
			threshold(7.0 / 1.49, 1200.0 / 1.49);
			for(unsigned j = 0; j < NumberPixels(); ++j)
			{
				pixels[j] = log10(pixels[j]);
			}
		break;
		
		// bytscl(  sqrt((image*(4.99803/exptime)>(  10.0/ 1.49 )<( 6000/ 1.49 ))))
		case 171:
			mul(4.99803/exposureTime);
			threshold(10.0 / 1.49, 6000.0 / 1.49);
			for(unsigned j = 0; j < NumberPixels(); ++j)
			{
				pixels[j] = sqrt(pixels[j]);
			}
		break;
		
		// bytscl(alog10((image*(2.9995/exptime) > ( 120d/ 2.2 ) < (6000d/ 2.2 ))))
		case 193:
			mul(2.99950/exposureTime);
			threshold(120.0 / 2.2, 6000.0 / 2.2);
			for(unsigned j = 0; j < NumberPixels(); ++j)
			{
				pixels[j] = log10(pixels[j]);
			}
		break;
		
		// bytscl(alog10((image*(4.99801/exptime)>(  30.0/ 1.10 )<(13000/ 1.10 ))))
		case 211:
			mul(4.99801/exposureTime);
			threshold(30.0 / 1.10, 13000.0 / 1.10);
			for(unsigned j = 0; j < NumberPixels(); ++j)
			{
				pixels[j] = log10(pixels[j]);
			}
		break;

		// bytscl(alog10((image*(4.99941/exptime)>(  50.0/12.11 )<( 2000/12.11  ))))
		case 304:
			mul(4.99941/exposureTime);
			threshold(50.0 / 12.11,  2000.0 / 12.11);
			for(unsigned j = 0; j < NumberPixels(); ++j)
			{
				pixels[j] = log10(pixels[j]);
			}
		break;
		
		// bytscl(alog10((image*(6.99734/exptime)>(   3.5/ 2.97 )<( 1000/ 2.97 ))))
		case 335:
			mul(6.99734/exposureTime);
			threshold(3.5 / 2.97, 1000.0 / 2.97);
			for(unsigned j = 0; j < NumberPixels(); ++j)
			{
				pixels[j] = log10(pixels[j]);
			}
		break;
		
		// bytscl(       (image*(2.99911/exptime)), max = 200, min=-8)
		case 1600:
			mul(2.99911/exposureTime);
			threshold(0, 200);
		break;
		
		// bytscl(       (image*(1.00026        )), max = 2500)
		case 1700:
			mul(1.00026);
			threshold(0, 2500);
		break;

		// bytscl(       (image*(1.00026/exptime)<(26000.)))
		case 4500:
			mul(1.00026/exposureTime);
			threshold(0, 26000);
		break;
		
		default:
			cerr<<"Unknow wavelength "<<wavelength<<" for AIA"<<endl;
			EUVImage::enhance_contrast();
		break;
	}
}


vector<unsigned char> AIAImage::color_table() const
{
	switch (int(wavelength))
	{
		case 94:
		{
			unsigned char colorTable[][3] = CT_AIA_94;
			return vector<unsigned char> (colorTable[0], colorTable[0] + 3*(sizeof(colorTable)/sizeof(colorTable[0])));
			break;
		}
		
		case 131:
		{
			unsigned char colorTable[][3] = CT_AIA_131;
			return vector<unsigned char> (colorTable[0], colorTable[0] + 3*(sizeof(colorTable)/sizeof(colorTable[0])));
			break;
		}
		
		case 171:
		{
			unsigned char colorTable[][3] = CT_AIA_171;
			return vector<unsigned char> (colorTable[0], colorTable[0] + 3*(sizeof(colorTable)/sizeof(colorTable[0])));
			break;
		}
		
		case 193:
		{
			unsigned char colorTable[][3] = CT_AIA_193;
			return vector<unsigned char> (colorTable[0], colorTable[0] + 3*(sizeof(colorTable)/sizeof(colorTable[0])));
			break;
		}
		
		case 211:
		{
			unsigned char colorTable[][3] = CT_AIA_211;
			return vector<unsigned char> (colorTable[0], colorTable[0] + 3*(sizeof(colorTable)/sizeof(colorTable[0])));
			break;
		}

		case 304:
		{
			unsigned char colorTable[][3] = CT_AIA_304;
			return vector<unsigned char> (colorTable[0], colorTable[0] + 3*(sizeof(colorTable)/sizeof(colorTable[0])));
			break;
		}
		
		case 335:
		{
			unsigned char colorTable[][3] = CT_AIA_335;
			return vector<unsigned char> (colorTable[0], colorTable[0] + 3*(sizeof(colorTable)/sizeof(colorTable[0])));
			break;
		}
		
		case 1600:
		{
			unsigned char colorTable[][3] = CT_AIA_1600;
			return vector<unsigned char> (colorTable[0], colorTable[0] + 3*(sizeof(colorTable)/sizeof(colorTable[0])));
			break;
		}
		
		case 1700:
		{
			unsigned char colorTable[][3] = CT_AIA_1700;
			return vector<unsigned char> (colorTable[0], colorTable[0] + 3*(sizeof(colorTable)/sizeof(colorTable[0])));
			break;
		}

		case 4500:
		{
			unsigned char colorTable[][3] = CT_AIA_4500;
			return vector<unsigned char> (colorTable[0], colorTable[0] + 3*(sizeof(colorTable)/sizeof(colorTable[0])));
			break;
		}
		
		default:
			cerr<<"Unknow wavelength "<<wavelength<<" for AIA"<<endl;
			return vector<unsigned char>(0);
		break;
	}
}

bool isAIA(const Header& header)
{
	return header.has("INSTRUME") && header.get<string>("INSTRUME").find("AIA") != string::npos;
}
