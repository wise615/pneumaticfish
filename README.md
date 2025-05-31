# PneumaticFish - UCLA ENGR 1UW Final Project

A Biomimetic Underwater Robot Designed to Navigate an Obstacle Course, Outfitted With a Dual-Pneumatic Tail and Four Degrees of Freedom, Controlled by Arduino Mega 2560!


## Table of Contents

* [Description](#description)
* [Materials and Robot Setup](#materials-and-robot-setup)
* [Usage](#usage)

## Description

**PneumaticFish** is an underwater robot controlled by an Arduino Mega 2560, designed to navigate an underwater obstacle course. Designed with the constraints set for the ENGR 1UW course at UCLA, this robot features three motors and a dual-pneumatic tail, enabling it to move with four degrees of freedom. <br/>

**PneumaticFish** has a 3D-printed chassis, with three motors mounted at the head, center, and tail. The tail is printed in _**x**_ parts, with pneumatic tubes running lengthwise through the entire tail and silicone soft pneumatic actuators inserted in between each tail segment. 

## Materials and Robot Setup

**PneumaticFish** requires the following materials in its setup: 
* 2x 1/4" 12V 2-Position 3-Way Solenoid Valve [Link](https://a.co/d/4U33Og6)
* 3x Bilge Pump Motor _**Need Link**_
* 1x Arduino Mega 2560 [Link](https://a.co/d/d7fq8C4)
* 1x Solderless Breadboard [Link](https://a.co/d/1CJI7wi)
* 2x Dual-Axis Joystick Module [Link](https://a.co/d/fT2if6U)
* 2x H-Bridge Dual Motor Drive Module [Link](https://a.co/d/jiryVRq)
* 2x 5V DC Relay Module [Link](https://a.co/d/7KeYKjm)
* Arduino-Compatible Jumper Cables [Link](https://a.co/d/d7FMUgr)
* Ecoflex 00-30 Silicone Rubber [Link](https://a.co/d/eTcQRdR)
* 1/4" Pneumatic Tubing [Link](https://a.co/d/9IjC0PE)
* 1x 1/4" Pneumatic Tee [Link](https://a.co/d/dHwe9kc)
* 12V External Power Supply
* Air Compressor and Regulator
* 3D Printer

When viewed from above with the head pointing towards the positive y-axis and the positive z-axis pointing towards the viewer, **PnuematicFish** contains three motors: two oriented towards the positive z-axis at the head and center, and one oriented towards the positive y-axis at the tail end. The pneumatic tubes that run along the left and right sides of the tail allow the tail to rotate in the xy-plane, allowing **PneumaticFish** to be steered in the x-axis. Air is diverted into both tubes from a single air source through a tee junction, then controlled into each side of the tail via two solenoids. The z-oriented motors power z-translational and z-angular motion, while the y-oriented motor enables xy-motion in coordination with the dual-pneumatic tail. <br/>

When setting up the Arduino Mega 2560, connect each electrical component according to the pin assignments detailed in the first several lines of the Arduino code. Note that the specific pins into which electrical components are plugged are flexible as long as the pin assignments in the code are updated accordingly and the change remains within the original DIGITAL/ANALOG/PWM pin regions. Because the Arduino can only provide 5V from its pins, the solenoids and motors must be plugged into relay and H-bridge modules, respectively, each powered by an external 12V power supply. 

## Usage

Usage of **PneumaticFish** is straightforward. 
1. Download `pneumaticfish.cpp` and rename the file extension to `pneumaticfish.ino`.
2. Open `pneumaticfish.ino` in Ardunio IDE (Download [here](https://www.arduino.cc/en/software/) if you haven't already).
3. Connect the Ardunio Mega 2560 to all of the electrical modules according to the setup described in [Materials and Robot Setup](#materials-and-robot-setup).
4. Connect the Ardunio Mega 2560 to your computer using a USB 2.0 Type B cable. Make sure "Ardunio Mega 2560" is selected as the COM port in Ardunio IDE.
5. Upload the code to the microcontroller and enjoy!

One joystick controls xy-motion while the other joystick controls z-motion. The xy-joystick thus simultaneously controls the tail motor and the dual-pneumatic tail, depending on which direction the joystick is moved. The z-joystick can be toggled to control either the z-translational motor or the z-angular motor by clicking the pushbutton. Only one dimension of the z-joystick is used. All motors support analog (graduated) control, while the dual-pneumatic tail only supports three states: left, straight, and right. 
