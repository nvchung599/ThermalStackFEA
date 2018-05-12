// Contains and executes element-element relationships
// In the case of thermal FEA, heat transfer

#include "MeshNode.h"
#include <iostream>

MeshNode::MeshNode(MeshElement * firstIn, MeshElement * secondIn)
{
	first = firstIn;
	second = secondIn;

	first->rememberNeighbor(second);
	second->rememberNeighbor(first);

	if (first->getZLayer() == second->getZLayer()) {
		resistanceAbsolute = first->getXYRAbsolute() + second->getXYRAbsolute();
	}
	else {
		resistanceAbsolute = first->getZRAbsolute() + second->getZRAbsolute();
	}
}

MeshNode::~MeshNode()
{
}

void MeshNode::calcEnergyTransfer(double timeStep)
{
	double dT = first->getTemperature() - second->getTemperature();
	double q = dT / resistanceAbsolute;
	double energy = q * timeStep;

	//std::cout << "first temp " << first->getTemperature() << std::endl;
	//std::cout << "second temp " << second->getTemperature() << std::endl;

	first->setPendingEnergy(-energy);
	second->setPendingEnergy(energy);
}

