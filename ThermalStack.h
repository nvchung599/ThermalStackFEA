// Contains a 3D heat transfer system and the means to solve for its transient and steady-state characteristics.
// 
// Example Usage:
//
//		ThermalStack myThermalCircuit( *inputs* );
// 
//		myThermalCircuit.addBlock( *inputs for heat source* );
//		myThermalCircuit.addBlock( *inputs for intermediate material* );
//		myThermalCircuit.addBlock( *inputs for infinite heatsink* );
//
//		myThermalCircuit.mesh();
//		myThermalCircuit.monitorBlock(0);
//		myThermalCircuit.solve();

#pragma once
#include "Block.h"
#include "MeshElement.h"
#include "MeshNode.h"
#include <vector>

class ThermalStack
{

public:

	// Solver parameters are defined in the class constructor
	ThermalStack(double meshSizeIn,
				 double timeStepIn,
				 int sampleIntervalStepsIn,
				 double deltaTConvergenceThresholdIn,
				 double startingTemperatureIn);

	~ThermalStack();

	// Creates a new rectangular material mass/block -- and pushes it onto one end of the thermal stack.
	// All blocks are centered in the X and Y
	// Works like stack::push()
	void addBlock(double xIn, double yIn, double zIn, Material materialIn, double qGenBlockIn);

	// Prepares the user-defined block stackup for simulation
	void mesh();

	// Specifies which block to monitor for convergence
	// Block must be a heat source
	void monitorBlock(int blockIndexIn);

	// March the solution, outputs useful data
	void solve();

private:

	void initElementArray();

	MeshElement * nav3DArray(int x, int y, int z);

	void genMeshElements();

	std::vector<MeshElement *> findNeighborElements(int x, int y, int z);

	void genMeshNodes();

	void illustrate();

	int locateTauStep(double tempInitial, double tempSteady);

	// Solver and mesh parameters
	double currTime;
	double meshSize;
	double timeStep;
	int sampleIntervalSteps;
	double deltaTConvergenceThreshold;
	double startingTemperature;
	double previousTemperature;
	int blockIndex;
	std::vector<double> tempHistory;

	// 3D object stuff
	std::vector<Block> blocks;
	int xElementCountMax;
	int yElementCountMax;
	int zElementCountMax;
	int activeElementCount;
	int totalElementCount;
	MeshElement * elementArray;
	std::vector<MeshNode> nodeVector;
};

