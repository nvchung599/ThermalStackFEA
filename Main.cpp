//
//			-----				 Thermal Stack FEA				---------
//		  ---------				  by Nathan Chung				  -----
//			-----				   May 11, 2018					---------
// 
// For more details on how this simulator works, see www.github.com/nvchung599/ThermalStackFEA
//
// For input data abstractions and unit conversions, see the included excel spreadsheet
//
// Main.cpp here serves as a user space for defining and simulating physical systems
//
// Follow the example below to construct your own model

#include "ThermalStack.h"
#include "Material.h"
#include <string>
#include <iostream>

using namespace std;

int main() {

	// define mesh and solver parameters here
	const double meshSize = 0.5;						// mm
	const double timeStep = 0.0001;						// sec
	const double sampleIntervalSteps = 10;				// QTY timeSteps
	const double deltaTConvergenceThreshold = 0.0001;	// degrees C
	const double startingTemperature = 65;				// degrees C

	// define material properties here
	// inputs are (conductivity k [W/mmK], volumetric heat capacity [J/mm^3K], material name)
	Material silicon(0.148, 0.001643,	"Silicon ");	
	Material aluminum(0.205, 0.002424,	"Aluminum");
	Material copper(0.401, 0.003450,	"Copper  ");
	Material tim(0.01, 0.003476,		"TIM Pad ");
	Material water(0.01, 20000,			"Water   ");
	//				 ^	   ^				note about water parameters:
	// artificial k to achieve desired convective heat transfer coefficient (see excel sheet)
	// artificial c to behave as infinite heat sink
	// c value must be <32767 roughly to avoid fucking with some int variable somewhere

	ThermalStack semiconductorSandwich(meshSize, 
									   timeStep, 
									   sampleIntervalSteps, 
									   deltaTConvergenceThreshold,
									   startingTemperature);

	// Define your physical system here
	// Blocks are stacked in order of initialization
	// Inputs are (xLength [mm], yLength [mm], zDepth [mm], material object, heat gen [W])

	semiconductorSandwich.addBlock(15, 15, meshSize, water, 0);	// block 0		---------------
	semiconductorSandwich.addBlock(15, 15, 3, aluminum, 0);		// block 1		---------------
	semiconductorSandwich.addBlock(10, 10, 0.5, tim, 0);		// block 2		   ---------
	semiconductorSandwich.addBlock(10, 10, 2, copper, 0);		// block 3		   ---------
	semiconductorSandwich.addBlock(5, 5, 1, silicon, 100);		// block 4			 -----
	semiconductorSandwich.addBlock(10, 10, 2, copper, 0);		// block 5		   ---------			
	semiconductorSandwich.addBlock(10, 10, 0.5, tim, 0);		// block 6		   ---------
	semiconductorSandwich.addBlock(15, 15, 3, aluminum, 0);		// block 7		---------------
	semiconductorSandwich.addBlock(15, 15, meshSize, water, 0);	// block 8		---------------

	// Generates a 3D model in which material masses are divided into discrete, cubic/rectangular elements
	// Prepares a linear datastructure of element associations for calculating heat transfer physics
	semiconductorSandwich.mesh();

	// Specify block for convergence monitoring -- block must be a heat source
	semiconductorSandwich.monitorBlock(4);

	// March the solution and output data realtime and post-convergence
	semiconductorSandwich.solve();

	// Pause
	string input;
	cin >> input;
}