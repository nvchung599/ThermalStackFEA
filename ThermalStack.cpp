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

#include "ThermalStack.h"
#include <iostream>
#include <math.h>
#include <iomanip>
#include <ctime>
#include <cstdlib>

ThermalStack::ThermalStack(double meshSizeIn,
						   double timeStepIn,
						   int sampleIntervalStepsIn,
						   double deltaTConvergenceThresholdIn,
						   double startingTemperatureIn) {
	meshSize = meshSizeIn;
	timeStep = timeStepIn;
	sampleIntervalSteps = sampleIntervalStepsIn;
	deltaTConvergenceThreshold = deltaTConvergenceThresholdIn;
	startingTemperature = startingTemperatureIn;
	previousTemperature = startingTemperature;

	currTime = timeStep; // just above zero... for reasons

	xElementCountMax = 0;
	yElementCountMax = 0;
	zElementCountMax = 0;
	activeElementCount = 0;
	totalElementCount = 0;

	blockIndex = 0;
}

ThermalStack::~ThermalStack()
{
	delete [] elementArray;
}

// Creates a new, user-defined, rectangular material mass -- and pushes it onto one end of the thermal stack.
// Works like stack::push()
void ThermalStack::addBlock(double xIn, double yIn, double zIn, Material materialIn, double qGenBlockIn)
{
	blocks.push_back(Block(xIn, yIn, zIn, meshSize, materialIn, qGenBlockIn));
}

// Generates a 3D model in which material masses are divided into discreet, cubic/rectangular elements.
// Prepares a linear datastructure for calculating heat transfer physics.
void ThermalStack::mesh()
{
	initElementArray();
	genMeshElements();
	genMeshNodes();

	int numBlockElementsVector = 0;
	int numBlockElementsXYZ = 0;

	for (int i = 0; i < blocks.size(); i++) {
		numBlockElementsVector += blocks[i].getElementVectorCount();
		numBlockElementsXYZ += blocks[i].getXElementCount() * blocks[i].getYElementCount() * blocks[i].getZElementCount();
	}
}

// Establishes 3D array dimensions that envelope all blocks
// The 3D object is stored in a flat array, necessitating the function "nav3DArray"
void ThermalStack::initElementArray()
{
	for (unsigned int i = 0; i < blocks.size(); i++) {

		if (this->xElementCountMax < blocks[i].getXElementCount()) {
			this->xElementCountMax = blocks[i].getXElementCount();
		}

		if (this->yElementCountMax < blocks[i].getYElementCount()) {
			this->yElementCountMax = blocks[i].getYElementCount();
		}

		this->zElementCountMax += blocks[i].getZElementCount();
	}

	unsigned int flat3DArraySize = this->xElementCountMax * this->yElementCountMax * this->zElementCountMax;
	elementArray = new MeshElement[flat3DArraySize];

	totalElementCount = xElementCountMax * yElementCountMax * zElementCountMax;
}

// Accesses the 3D object/array and returns a pointer to an element object
// Returns nullptr if the coordinate does not hold an active element
MeshElement * ThermalStack::nav3DArray(int x, int y, int z)
{
	if (x < 0 || x >= xElementCountMax ||
		y < 0 || y >= yElementCountMax || 
		z < 0 || z >= zElementCountMax) {

		return nullptr;
	}

	MeshElement * elementPtr;
	elementPtr = &elementArray[x + (y * xElementCountMax) + (z * xElementCountMax * yElementCountMax)];

	if (elementPtr->isEmpty()) {
		return nullptr;
	}
	
	return elementPtr;
}

// Converts the block stackup into their associated elements in the 3D array
// The material masses are centered on eachother
void ThermalStack::genMeshElements()
{
	std::cout << "Generating mesh elements... ";

	int currBlock = 0;
	int currBlockRemainingLayers = blocks[currBlock].getZElementCount();

	for (int z = 0; z < zElementCountMax; z++) {
		for (int x = 0; x < xElementCountMax; x++) {
			for (int y = 0; y < yElementCountMax; y++) {

				// create material elements for this layer/block
				MeshElement * elementPtr = nav3DArray(x, y, z);
				*elementPtr = MeshElement(startingTemperature,
										  blocks[currBlock].getQGenElement() * timeStep,
										  blocks[currBlock].getCElement(),
										  blocks[currBlock].getXYRAbsolute(),
										  blocks[currBlock].getZRAbsolute(),
										  z);

				// if coordinate not in centered block domain, set as empty/inactive
				// otherwise, record as an active element
				int xStart = (xElementCountMax - blocks[currBlock].getXElementCount()) / 2;
				int yStart = (yElementCountMax - blocks[currBlock].getYElementCount()) / 2;
				int xEnd = xStart + blocks[currBlock].getXElementCount() - 1;
				int yEnd = yStart + blocks[currBlock].getYElementCount() - 1;

				if (x < xStart	|| 
					x > xEnd	|| 
					y < yStart	|| 
					y > yEnd) {
					elementPtr->makeEmpty();
				}
				else {
					blocks[currBlock].rememberMyElement(elementPtr);
					activeElementCount++;
				}
			}
		}

		currBlockRemainingLayers--;
		if (currBlockRemainingLayers == 0) {
			currBlock++;
			if (currBlock < blocks.size()) {
				currBlockRemainingLayers = blocks[currBlock].getZElementCount();
			}
		}

	}

	std::cout << "Generated " << activeElementCount << " elements" << std::endl;
}

// Finds all active elements that are adjacent to this coordinate
std::vector<MeshElement *> ThermalStack::findNeighborElements(int x, int y, int z)
{
	std::vector<MeshElement *> neighbors;
	MeshElement * tempPtr;

	int neighborCount = 0;

	tempPtr = nav3DArray(x + 1, y, z);
	if (tempPtr != nullptr) {
		neighbors.push_back(tempPtr);
		neighborCount++;
	}

	tempPtr = nav3DArray(x - 1, y, z);
	if (tempPtr != nullptr) {
		neighbors.push_back(tempPtr);
		neighborCount++;
	}

	tempPtr = nav3DArray(x, y + 1, z);
	if (tempPtr != nullptr) {
		neighbors.push_back(tempPtr);
		neighborCount++;
	}

	tempPtr = nav3DArray(x, y - 1, z);
	if (tempPtr != nullptr) {
		neighbors.push_back(tempPtr);
		neighborCount++;
	}

	tempPtr = nav3DArray(x, y, z + 1);
	if (tempPtr != nullptr) {
		neighbors.push_back(tempPtr);
		neighborCount++;
	}

	tempPtr = nav3DArray(x, y, z - 1);
	if (tempPtr != nullptr) {
		neighbors.push_back(tempPtr);
		neighborCount++;
	}

	//std::cout << "neighborCount " << neighborCount << std::endl;
	return neighbors;
}

// Creates element-to-element conduction paths and stores these links in a linear datastructure for ease of processing
void ThermalStack::genMeshNodes()
{
	std::cout << "Creating element links/nodes... ";

	MeshElement * currElementPtr;

	for (int z = 0; z < zElementCountMax; z++) {
		for (int x = 0; x < xElementCountMax; x++) {
			for (int y = 0; y < yElementCountMax; y++) {

				currElementPtr = nav3DArray(x, y, z);

				if (currElementPtr != nullptr) {
					std::vector<MeshElement *> currNeighbors;
					currNeighbors = findNeighborElements(x, y, z);

					for (int i = 0; i < currNeighbors.size(); i++) {
						if (currNeighbors[i]->checkForExistingNode(currElementPtr) == false) {
							nodeVector.push_back(MeshNode(currElementPtr, currNeighbors[i]));
						}
					}
				}
			}
		}
	}

	std::cout << "Created " << nodeVector.size() << " nodes" << std::endl;
}

// Establishes which block/material mass will be monitored for convergence
void ThermalStack::monitorBlock(int blockIndexIn)
{
	blockIndex = blockIndexIn;
}

// Outputs a 2D visual of the thermal stack with useful data for each layer
void ThermalStack::illustrate()
{
	std::cout << std::fixed;
	std::cout << std::setprecision(0);

	std::cout << "                                         Matl        T_avg      T_var     Q_gen     Vol \n\n";

	for (int i = 0; i < blocks.size(); i++) {

		double doublefiedElementCount = blocks[i].getXElementCount();
		double normalizedX = doublefiedElementCount / xElementCountMax;
		int dashCount = round(normalizedX * 10) * 2; // max is 20
		int dashStartIndex = (20 - dashCount) / 2;
		int dashEndIndex = 20 - ((20 - dashCount) / 2);

		std::cout << "    Block " << i << "\t";
		for (int j = 0; j < 20; j++) {
			if (j >= dashStartIndex && j < dashEndIndex) {
				std::cout << "-";
			}
			else {
				std::cout << " ";
			}
		}
		std::cout << "  \t " << blocks[i].getMaterialName() << "  "
				  << "  " << blocks[i].getBulkTemp() << " C"
				  << "\t" << blocks[i].getTempNonUniformity() << " C"
			      << "\t  " << blocks[i].getQGen() << " W"
				  << "\t    " << blocks[i].getVolume() << " mm^3";
		std::cout << "\n";
	}
}

// Upon reaching a steady-state solution, this method crawls historical data and locates the instance at t = 1 * time constant
int ThermalStack::locateTauStep(double tempInitial, double tempSteady)
{
	double dTTotal = tempSteady - tempInitial;
	double dTAtTauOne = dTTotal * 0.368;
	int tauStep = 0;

	for (int i = 0; i < tempHistory.size(); i++) {
		tauStep++;
		if ((tempSteady - tempHistory[i]) < dTAtTauOne) {
			break;
		}
	}

	return tauStep;
}

// Marches the solution to convergence, outputs data realtime, outputs report after converging
void ThermalStack::solve() 
{

	clock_t startTime = clock(); //Start timer

	std::cout << "\nThermal stack initial state:\n\n";
	illustrate();
	std::cout << "\n";

	std::cout << std::fixed;
	std::cout << std::setprecision(0);

	std::cout << "Solving...\n\n";
	std::cout << "    Monitoring block " << blockIndex << ", " << blocks[blockIndex].getMaterialName()
		<< ", generating " << blocks[blockIndex].getQGen() << " W\n";

	std::cout << std::setprecision(2);
	std::cout << "    Mesh Size = " << meshSize << " mm\n";
	std::cout << std::setprecision(6);
	std::cout << "    Time Step = " << timeStep << " sec\n";
	std::cout << "    Sampling Time Inverval = " << timeStep * sampleIntervalSteps << " sec\n";
	std::cout << std::setprecision(3);
	std::cout << "    Convergence dT/dt_Target = " << deltaTConvergenceThreshold / (timeStep * sampleIntervalSteps) << " C/sec\n\n";
	std::cout << "    t = " << 0 << " seconds         T_avg = " << startingTemperature << " C\n";

	bool haveIConvergedYet = false;
	int currStep = 0;
	int tauInterval = 0;
	double currMonitoredTemperature = 0;

	while (haveIConvergedYet == false) {

		for (int i = 0; i < nodeVector.size(); i++) { 
			nodeVector[i].calcEnergyTransfer(timeStep);
		}

		for (int j = 0; j < totalElementCount; j++) {
			elementArray[j].applyEnergyTransfer();
		}

		currTime += timeStep;
		currStep = currTime / timeStep;

		if (currStep % sampleIntervalSteps == 0) {

			currMonitoredTemperature = blocks[blockIndex].getBulkTemp();
			tempHistory.push_back(currMonitoredTemperature);

			if ((currMonitoredTemperature - previousTemperature) > deltaTConvergenceThreshold) {
				std::cout << "                                                                                                           \r";
				std::cout << "    t = " << currTime << " seconds     T_avg = " << currMonitoredTemperature << " C"
					<< "\tdT/dt_Current = " << (currMonitoredTemperature - previousTemperature) / (timeStep * sampleIntervalSteps) << " C/sec\r";
				std::cout.flush();
			}
			else {

				tauInterval = locateTauStep(startingTemperature, currMonitoredTemperature);
				double tauTime = tauInterval * sampleIntervalSteps * timeStep;
				std::cout << "                                                                                                           \r";
				std::cout << "    t = " << tauTime << " seconds     T_avg = " << tempHistory[tauInterval] << " C"
					<< "  \t<- @ one time constant\n";
				std::cout << "    t = " << currTime << " seconds     T_avg = " << currMonitoredTemperature << " C"
					<< "  \t<- @ steady state\n";

				int secondsElapsed = (clock() - startTime) / CLOCKS_PER_SEC;
				int minutesElapsed = floor(secondsElapsed / 60);
				int secondsRemainder = secondsElapsed % 60;
				
				std::cout << "\nConverged on the following solution after " 
					<< minutesElapsed << " minutes and "  << secondsRemainder << " seconds";
				haveIConvergedYet = true;
			}
			previousTemperature = currMonitoredTemperature;
		}
	}

	std::cout << "\n\n";
	illustrate();
	std::cout << "\n";

	double thermalImpedance = (currMonitoredTemperature - startingTemperature) / blocks[blockIndex].getQGen();

	std::cout << std::fixed;
	std::cout << std::setprecision(3);
	std::cout << "Thermal impedance, heat source to infinite heatsink = " << thermalImpedance << " K/W \n\n";
}