// Represents a user-defined rectangular material mass.
// Multiple blocks of differing dimensions can be stacked together to form more complex heat transfer circuits

#include "Block.h"
#include <iostream>
#include <string>
#include <cmath>


// X and Y dimensions are rounded to the nearest mm
Block::Block(double xIn, double yIn, double zIn, double meshSizeIn, Material materialIn, double qGenBlockIn)
{
	xLength = round(xIn);
	yLength = round(yIn);
	zLength = zIn;
	k = materialIn.k;
	c = materialIn.c;
	materialName = materialIn.name;
	qGenBlock = qGenBlockIn;

	this->genMeshDimensions(meshSizeIn);
	this->calcElementProperties();
}

Block::~Block()
{
}

void Block::genMeshDimensions(double meshSize) {
	xElementCount = round(xLength / meshSize);
	yElementCount = round(yLength / meshSize);
	zElementCount = ceil(zLength / meshSize);

	//std::cout << xElementCount << std::endl;
	//std::cout << yElementCount << std::endl;
	//std::cout << zElementCount << std::endl;
}

// Transforms user-input material properties for the specified mesh size
void Block::calcElementProperties()
{
	// mm^3
	double blockVolume = xLength * yLength * zLength;

	int numElements = xElementCount * yElementCount * zElementCount;

	cElement = (c * blockVolume) / numElements;

	qGenElement = qGenBlock / numElements;

	double xyLengthElement = xLength / xElementCount; // square XY
	double zLengthElement = zLength / zElementCount;
	double sideAreaElement = xyLengthElement * zLengthElement;
	double verticalAreaElement = xyLengthElement * xyLengthElement;

	xyRAbsolute = (xyLengthElement / 2) / (k * sideAreaElement);
	zRAbsolute = (zLengthElement / 2) / (k * verticalAreaElement);
}

void Block::rememberMyElement(MeshElement * elementPtr)
{
	blockElements.push_back(elementPtr);
}

// Calculates mean temperature of this block
double Block::getBulkTemp()
{
	double temperatureSum = 0;

	for (int i = 0; i < blockElements.size(); i++) {
		temperatureSum += blockElements[i]->getTemperature();
	}

	return (temperatureSum / blockElements.size());

}

// Calculates StdDev of temperature distribution from all the elements
double Block::getTempStandardDeviation()
{
	
	double doublifiedElementCount = getElementVectorCount();
	double meanTemp = getBulkTemp();
	double sumSquareErrors = 0;
	for (int i = 0; i < doublifiedElementCount; i++) {
		double elementTemp = blockElements[i]->getTemperature();
		sumSquareErrors += pow((elementTemp - meanTemp), 2);
	}

	double tempStandardDeviation = sqrt(sumSquareErrors) / doublifiedElementCount;

	//std::cout << "\n";
	//std::cout << "tempStandardDeviation " << tempStandardDeviation << std::endl;
	//std::cout << "sqrt(sumSquareErrors) " << sqrt(sumSquareErrors) << std::endl;
	//std::cout << "getElementVectorCount() " << doublifiedElementCount << std::endl;

	return tempStandardDeviation;
}

// Finds the temperature difference between the hottest and coldest element in this block
double Block::getTempNonUniformity()
{
	double low = getBulkTemp();
	double high = getBulkTemp();

	for (int i = 0; i < getElementVectorCount(); i++) {
		double elTemp = blockElements[i]->getTemperature();
			if (elTemp < low) {
				low = elTemp;
			}
			if (elTemp > high) {
				high = elTemp;
			}
	}

	return (high - low);
}

// Accessors
std::string Block::getMaterialName() { return materialName; }
double Block::getQGen() { return qGenBlock; }
double Block::getQGenElement() { return qGenElement; }
double Block::getCElement() { return cElement; }
double Block::getXYRAbsolute() { return xyRAbsolute; }
double Block::getZRAbsolute() { return zRAbsolute; }
double Block::getXLength() { return yLength; }
double Block::getYLength() { return yLength; }
double Block::getVolume() { return (xLength * yLength * zLength); }
int Block::getXElementCount() { return xElementCount; }
int Block::getYElementCount() { return yElementCount; }
int Block::getZElementCount() { return zElementCount; }
int Block::getElementVectorCount() { return blockElements.size(); }