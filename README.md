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

__---------------------------------------------------------------------------
A follow up: I used the translator to update a design previously imported into KiCAD.

Encountered some problems:
1) To allow users to embedded whitespace into component values I read the OrCAD value field with getline()
as the value would appears as two (or more tokens), e.g. 1uF50V versus 1uF 50V. HOWEVER, PCBnew (KiCAD's 
layout tool) balked at the embedded white space. The error messages from PCBnew located the "error" and was
repaired with a text editor.
2) KiCAD replaces user netnames internally with a node number. When updating the netlist the KiCAD layout was
hopelessly scramlbled and I exited PCBnew without saving the changes. On the second try I clicked the radio
button "Re-associated footprints by refernce" (as apposed to "Keep existing symbol to footprint associations")
and the update when smoothly.

To avoid associating new footprints by hand (using a text editor) I "preloaded" the new footprints in PCBnew
using the <Add footprint> command off the right hand menu. The generic reference (as loaded from the library)
was updated to the new references imported from OrCAD Capture. Of course, one could use a text editor for this
operation on the translated netlist, but with only a few changes, the <Add footprint> command was handy. The
tool allows the user to brows previously associated libraries and pick the footprint of interest.
  
  DE
  08feb2021
