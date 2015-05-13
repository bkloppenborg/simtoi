/*
 * CPhotometry.cpp
 *
 *  Created on: May 12, 2015
 *      Author: bkloppenborg
 */

#include "CPhotometry.h"
#include <iostream>
#include <fstream>

using namespace std;

#include "textio.hpp"

CPhotometry::CPhotometry() {
	// TODO Auto-generated constructor stub

}

CPhotometry::~CPhotometry() {
	// TODO Auto-generated destructor stub
}


CDataInfo CPhotometry::open(const string & filename)
{
	// The photometric files must conform to the following format:
	//	# comment lines prefixed by #, /, ;, or !
	//	JD,mag,sig_mag,ANYTHING_ELSE

	string line;
	double jd, value, uncertainty, wavelength;

	// Create a new data file for storing input data.
	mFilename = stripPath(filename);

	// Get a vector of the non-comment lines in the text file:
	vector<string> lines = ReadFile(filename, "#/;!", "Could not read photometric data file " + filename + ".");
	for(unsigned int i = 0; i < lines.size(); i++)
	{
		line = lines[i];
		line = StripWhitespace(line);
		vector<string> t_line = SplitString(line, ',');

		if(t_line.size() < 4)
		{
			cout << "WARNING: Could not parse '" + line + "' from photometric data file " + filename << endl;
			continue;
		}

		// Attempt to cast variables
		try
		{
			// extract the jd, value, uncertainty, and wavelength from the line
			jd = atof(t_line[0].c_str());
			value = atof(t_line[1].c_str());
			uncertainty = atof(t_line[2].c_str());
			wavelength = lookupWavelength(t_line[4]);

			// find the start, end, and mean Julian dates.
			mJDMean += jd;
			if(jd < mJDMin)
				mJDMin = jd;
			if(jd > mJDMax)
				mJDMax = jd;

			// find the start, end, and mean wavelengths dates.
			mWavelengthMean += wavelength;
			if(wavelength < mWavelengthMin)
				mWavelengthMin = wavelength;
			if(wavelength > mWavelengthMax)
				mWavelengthMax = wavelength;

			// Everything imported ok, append the data to the vectors
			mJDs.push_back(jd);
			mValues.push_back(value);
			mUncertainties.push_back(uncertainty);
			mWavelengths.push_back(wavelength);
		}
		catch(...)
		{
			cout << "WARNING: Could not parse '" + line + "' from photometric data file " + filename << endl;
		}
	}

	// compute the mean wavelength and JD
	mJDMean /= mJDs.size();
	mWavelengthMean /= mWavelengths.size();
}

/// Saves a minimal export of this data file to the specified directory
void CPhotometry::saveAsText(const string & filename)
{
	save(filename);
}

/// Save the photometry in the AAVSO compliant CSV format to the specifid file
void CPhotometry::save(const string & filename)
{
	ofstream savefile;
	savefile.open(filename);

	savefile.width(15);
	savefile.precision(8);

	for(unsigned int i = 0; i < size(); i++)
	{
		savefile << mJDs[i] << "," << mValues[i] << "," << mUncertainties[i] << "," << mWavelengths[i] << endl;
	}

	savefile.close();
}

/// Return the number of photometric points in this data file
unsigned int CPhotometry::size()
{
	return mValues.size();
}

/// Returns the wavelength, in meters, of the filter. If wavelength is
/// numeric, it is expected that the units are in meters.
double CPhotometry::lookupWavelength(const string & wavelength_or_band)
{
	double wavelength = -1;

	// First try to convert to a number directly
	wavelength = atof(wavelength_or_band.c_str());

	// Otherwise use one of the standard astronomical filters.
	// Remember, if you update these values make a change to the wiki
	if(wavelength_or_band == "U") wavelength = 365.0E-9;
	if(wavelength_or_band == "B") wavelength = 445.0E-9;
	if(wavelength_or_band == "V") wavelength = 551.0E-9;
	if(wavelength_or_band == "R") wavelength = 658.0E-9;
	if(wavelength_or_band == "I") wavelength = 806.0E-9;
	if(wavelength_or_band == "J") wavelength = 1.250E-6;
	if(wavelength_or_band == "H") wavelength = 1.635E-6;
	if(wavelength_or_band == "K") wavelength = 2.200E-6;
	if(wavelength_or_band == "L") wavelength = 3.450E-6;
	if(wavelength_or_band == "M") wavelength = 4.750E-6;
	if(wavelength_or_band == "N") wavelength = 10.500E-6;
	if(wavelength_or_band == "Q") wavelength = 21.000E-6;

	if(!(wavelength > 0))
		throw runtime_error("Could not determine wavelength");

	return wavelength;
}
