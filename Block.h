// Represents a user-defined rectangular material mass.
// Multiple blocks of differing dimensions can be stacked together to form more complex heat transfer circuits

#pragma once
#include <vector>
#include "Material.h"
#include "MeshElement.h"
class MeshElement;

class Block
{

public:

	Block(double xIn, double yIn, double zIn, double meshSizeIn, Material materialIn, double qGenBlockIn);

	~Block();

	void genMeshDimensions(double meshSize);

	void calcElementProperties();

	void rememberMyElement(MeshElement * elementPtr);

	double getBulkTemp();

	double getTempStandardDeviation();

	double getTempNonUniformity();

	std::string getMaterialName();
	double getQGen();
	double getQGenElement();
	double getCElement();
	double getXYRAbsolute();
	double getZRAbsolute();
	double getXLength();
	double getYLength();
	double getVolume();
	int getXElementCount();
	int getYElementCount();
	int getZElementCount();
	int getElementVectorCount();

private:

	std::string materialName;

	double xLength; 	// rounded length [mm]
	double yLength;

	double zLength;		// precision depth [mm]

	double k;			// material conductivity [W/mmK]

	double xyRAbsolute; // element absolute half resistance [K/W]
	double zRAbsolute;

	double c;			// material volumetric heat capacity [J/mm^3K]
	double cElement; 	// per-element heat capacity [J/K]

	double qGenBlock; 	// block heat gen [W]
	double qGenElement; // per-element heat gen [W]

	int xElementCount;	// dimensions in mesh element count
	int yElementCount;
	int zElementCount;

	std::vector<MeshElement *> blockElements;

};

