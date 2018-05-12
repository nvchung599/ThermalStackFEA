// Elements serve as the fundamental components of a meshed 3D model. They are cubic or rectangular prisms.
// Elements are square in the X and Y. Z height can be smaller than the mesh size, though this is not recommended.

#pragma once
#include <vector>

class MeshElement
{
public:
	
	MeshElement();
	MeshElement(double temperatureIn,
				double energyGenPerTimestepIn,
				double cElementIn,
				double xyRAbsoluteIn,
				double zRAbsoluteIn,
				int zLayerIn);
	~MeshElement();
	bool isEmpty();
	void makeEmpty();
	void rememberNeighbor(MeshElement * potentialNeighbor);
	bool checkForExistingNode(MeshElement * potentialNeighbor);
	int getZLayer();
	double getXYRAbsolute();
	double getZRAbsolute();
	double getTemperature();
	void setPendingEnergy(double energyTransfer);
	void applyEnergyTransfer();

private:

	bool empty;

	std::vector<MeshElement *> neighbors;

	double temperature;
	double energyGenPerTimestep;
	double cElement;				// J/K
	double xyRAbsolute;
	double zRAbsolute;
	int zLayer;
	double energyExternalPending;
};

