#pragma once
#include <string>

struct Material {
	
	Material(double kIn, double cIn, std::string nameIn) {
		c = cIn;
		k = kIn;
		name = nameIn;
	}

	// volumetric heat capacity, [J/mm^3K]
	double c;

	// thermal conductivity, [W/mmK]
	double k;

	std::string name;

};


