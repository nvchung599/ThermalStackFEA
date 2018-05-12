// Contains and executes element-element relationships
// In the case of thermal FEA, heat transfer

#pragma once
#include "MeshElement.h"

class MeshNode
{

public:
	MeshNode(MeshElement * firstIn, MeshElement * secondIn);
	~MeshNode();
	void calcEnergyTransfer(double timeStep);

private:

	MeshElement * first;
	MeshElement * second;

	// element-element thermal impedance
	double resistanceAbsolute;
};

