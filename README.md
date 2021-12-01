# TI Innovator HUB Data Aquisition Tool (TI-82)

## Table of Contents

* [Introduction](#introduction)
* [User Guide and Theory of Operation](#user-guide-and-theory-of-operation)
* [Folder Descriptions](#folder-descriptions)
* [Demonstration Videos](#demonstration-videos)
* [Project Pitch](#project-pitch)

## Introduction

### Background
Students often begin learning about electronics in an environment where there is no access to the needed equipment, such as oscilloscopes, logic analyzers, function generators, and multimeters.

### Objective
Our goal was to create an inexpensive add-on for the TI Innovator HUB that provides data acquisition capabilities, with an intuitive web page based graphical user interface that works across many user devices.

### Product Requirements
- 8 Channel logic analyzer
- DMM that measures current, voltage, power, resistance
- 2 Channel oscilloscope
- Sine, square, sawtooth function generator
- Web browser-based GUI
- Wi-Fi Connectivity

### Design Challenges Encountered
- Integrating firmware for each peripheral.
- Optimizing data stream for real time operation.
- Changing firmware to allow peripherals to communicate with each other.
- Improving error checking to prevent hangups.

##  User Guide and Theory of Operation

Please see [this folder](https://github.ncsu.edu/cedenni2/ti_82/tree/master/project_files).

## Folder Descriptions

- /esp32_firmware: This folder gives the rundown on the wifi module (ESP32). Contains the firmware and instructions on how to upload it to the ESP32 and run it with the TI Innovator HUB.
- /final_gui: This folder shows the code behind the web based GUI. This folder contains instructions on how you can modify the webpages, encode them, and update them on the ESP32.
- /project_files: This folder contains administrative documents such as BOM, budget, project planning, theory of operation, and user manual. It is important to note that how to use the project and how it works is contained in the documents in this folder.
- /schematics: This folder contains the CAD files used to create the PCBs for each module. The PCBs were designed using EagleCAD.
- /ti_innovator_hub_firmware: This folder contains information on the firmware that resides inside the TI Innovator HUB. How the software works is located in the theory of operation document.

## Demonstration Videos

- [Beta Demo Video](https://youtu.be/WWzbL9Egt7U)
- [Final Demo Video](https://www.youtube.com/watch?v=TzPDwndSMNo)

## Project Pitch

### Background of Company: 
Global semiconductor company that designs, manufactures, tests and sells analog and embedded processing chips. Our products help our customers efficiently manage power, accurately sense and transmit data and provide the core control or processing in their designs.

### Problem Background and the need:
Work on building a basic instrumentation tool for portable signal measurements using the classroom ready TI-Innovator Hub product. Create a basic oscilloscope out of a microcontroller for collecting test and measurement data! Over the course of the project, the team will use an existing microcontroller kit and create a GUI interface to showcase wave forms, logic analyzer data, and digital multimeter functions. It is very important for engineers to be able to take measurements when debugging hardware. While benchtop equipment is highly capable, it is not portable and can be very expensive. The best tools are often the ones that are available, so portable tools are always in demand, even if the performance is not the same as professional equipment. The team will create software that works with existing hardware to display information collected from the portable data acquisition tool. They will iterate the design to improve the user “out of box” experience. They will use Whole Product development philosophy to create the necessary marketing materials and software examples to ensure a positive experience with users. The team will also design or research any accessory components that will be included in the final kit, which could include a custom enclosure, connectors, cables, power supplies, stickers, and other necessary parts.

### Problem Description/requirements (List of high-level functionality you expect to see in the end product):
- Software GUI that runs in web browser or as native PC application
- GUI Composer app or HTML5 app
- Functional prototype
- TI-Innovator Hub
- Jumper Wires
- (optional) Additional sensors or peripherals (e.g. DAC, voltmeter)
- (optional) Measurement probes or scope probes
- Programmable by USB from host PC
- Test plan to verify use for academic settings
- Identify and test use cases for basic circuits
- Documentation on user manual and quick start guide
- Include pin map diagram for inputs
- Include color photo of final board
- Explanation of out of box demo or default program
- At least 1 example code samples with documentation
- Measurements displayed in GUI
- Can be based on TI code examples provided for MCU or other sources
- Make available for Code Composer Studio and/or Arduino IDE
- Written list or physical prototypes of accessories for the development board which could include a custom enclosure, connectors, cables, power supplies, stickers, and other necessary parts
