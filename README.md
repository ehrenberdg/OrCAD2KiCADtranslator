# OrCAD2KiCADtranslator
Program to assembly KiCAD S-expression netlist from OrCAD Tango netlist

For 30 years my tool path to create printed circuit boards (PCBs) was to do schematic capture with OrCAD Capture and then
use the PCB123 software from Sunstone Circuits to layout and manufacture the PC boards. Decades ago, the industry lacked
standards and the PCB123/Sunstone path was convenient as all the "back end mickey mouse" (e.g. Gerber version, drill file
standards) was hidden and managed by Sunstone. However, in the intervening 3 decades software packages such as KiCAD have
matured and being locked into PCB123 has become expensive, especially when conpared to overseas manufacture.

The problem: lack of translation of my extensive OrCAD library to a netlist that can be utilized by KiCAD. PCB123 input 
netlists and tool output are propietary and can only be used by Sunstone Circuits.

The solution: Write a program to translate the simple Tango netlist, utilized by PCB123, and one of several dozen
possible netlists formats created by OrCAD Capture, to the elaborate S-expression netlist found in KiCAD.
In this repository is a simple C program for this purpose. I have made an attempt to at error detection
on corrupt input files, but the vetting is not exhaustive. NOT INCLUDED is footprint association with KiCAD library
footprints. A good home key editor (such as vi, or vim) and decent typing skills can insert KiCAD footprint calls into
the output netlist. I might create a table driven program to do this function in the future.

DE 
01feb2021
