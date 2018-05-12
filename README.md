# ThermalStackFEA

ThermalStackFEA is a light-weight, 3D, finite-element-analysis tool
for simulating heat transfer in stacked material masses.

![solvedstack](https://github.com/nvchung599/ThermalStackFEA/blob/master/solvedstack.PNG)

Features include:
* Steady-state and transient solver
* 3D modeling and meshing
* 3D temperature gradient reporting
* Customizable convergence criteria
* Real-time convergence monitoring
* Material libraries

ThermalStackFEA is useful for studying rectangular slices/sections
of larger systems, or simulating fully defined systems consisting of
stacks of non-uniform rectangular masses.

## Getting Started

No external libraries needed.

This [visual guide](https://github.com/nvchung599/ThermalStackFEA/blob/master/ThermalStackFEA%20Illustrations.pdf)
should clarify ThermalStackFEA's workflow.

Main.cpp is the user space. It contains a documented example/template
that you may follow to construct and simulate your own heat transfer system.

The included [excel sheet](https://github.com/nvchung599/ThermalStackFEA/blob/master/ThermalStackFEA%20Parameters%20and%20Material%20Properties.xlsx)
contains unit conversions and calculators for resolving various input parameters.

Example output:

![exampleoutput](https://github.com/nvchung599/ThermalStackFEA/blob/master/exampleoutput.PNG)

## Tips

* Convective heat transfer can be approximated as conduction into a
solid of infinite heat capacity. The provided [excel sheet](https://github.com/nvchung599/ThermalStackFEA/blob/master/ThermalStackFEA%20Parameters%20and%20Material%20Properties.xlsx)
can transform a desired convective heat transfer coefficient (h) into an
equivalent conduction value (k) for use in your model. Calculate
h values separately for various extended surface geometries and feed
them into this simulator.
* Define new materials and their properties in the appropriate units using the
[excel sheet](https://github.com/nvchung599/ThermalStackFEA/blob/master/ThermalStackFEA%20Parameters%20and%20Material%20Properties.xlsx).
* Having trouble converging in a reasonable amount of time? Adjust your
mesh and convergence criteria with the help of the
[excel sheet](https://github.com/nvchung599/ThermalStackFEA/blob/master/ThermalStackFEA%20Parameters%20and%20Material%20Properties.xlsx).
Excel sheet is always the answer.
* Block dimensions are rounded to the nearest mm. To ensure that block
meshes are centered on one another, the quotients xLength/MeshSize and
yLength/MeshSize should both be even integers.
* For 1D heat transfer circuits, use mesh size 1mm, block XY 1mm^2



