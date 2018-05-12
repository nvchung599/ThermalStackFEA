// Elements serve as the fundamental components of a meshed 3D model. They are cubic or rectangular prisms.
// Elements are square in the X and Y. Z height can be smaller than the mesh size, though this is not recommended.

#include "MeshElement.h"
#include <iostream>
#include <algorithm>

// While an incompletely defined element should be considered "inactive" until fully defined, setting the element to not-empty
// by default is necessary to be able to access the element with ThermalStack::nav3DArray later on
MeshElement::MeshElement() { empty = false; };

MeshElement::MeshElement(double temperatureIn,
						 double energyGenPerTimestepIn,
						 double cElementIn,
						 double xyRAbsoluteIn,
						 double zRAbsoluteIn,
						 int zLayerIn)
{
	empty = false;
	energyExternalPending = 0;

	temperature = temperatureIn;
	energyGenPerTimestep = energyGenPerTimestepIn;
	cElement = cElementIn;
	xyRAbsolute = xyRAbsoluteIn;
	zRAbsolute = zRAbsoluteIn;
	zLayer = zLayerIn;
}

MeshElement::~MeshElement()
{
}

bool MeshElement::isEmpty()
{
	return empty; // bool
}

void MeshElement::makeEmpty()
{
	empty = true;
}

void MeshElement::rememberNeighbor(MeshElement * potentialNeighbor)
{
	neighbors.push_back(potentialNeighbor);
}

// checks if these two elements have been associated with eachother already
bool MeshElement::checkForExistingNode(MeshElement * potentialNeighbor)
{
	bool foundBool;
	foundBool = std::find(neighbors.begin(), neighbors.end(), potentialNeighbor) != neighbors.end();
	return foundBool;
}

int MeshElement::getZLayer()
{
	return zLayer;
}
double MeshElement::getXYRAbsolute()
{
	return xyRAbsolute;
}
double MeshElement::getZRAbsolute()
{
	return zRAbsolute;
}
double MeshElement::getTemperature()
{
	return temperature;
}

// lines up a calculated energy transfer value for the next time step
void MeshElement::setPendingEnergy(double energyTransfer)
{
	energyExternalPending += energyTransfer;
	//std::cout << "energyExternalPendingp " << energyExternalPending << std::endl;
}

// modifies this elements temperature/interal energy with the queued energy transfer value
void MeshElement::applyEnergyTransfer()
{
	if (this->empty) {
		return;
	}

	// sum internal and external energy components
	double energySum = energyGenPerTimestep + energyExternalPending;

	//std::cout << "energyGenPerTimestep" << energyGenPerTimestep << std::endl;
	//std::cout << "energyExternalPending" << energyExternalPending << std::endl;
	//std::cout << std::endl;

	// use ^^^ to modify element temp
	double dT = energySum / cElement;
	
	// dump the energy into this element
	temperature += dT;
	energyExternalPending = 0;
}
