<p align="center">
  <img src="docs/GauchoSatLogo.png" alt="GauchoSatLogo" width="600"/>
</p>

## Overview
GauchoSat is a CubeSat project aimed at advancing space solar cell technology through in-orbit testing and data acquisition. The satellite, a 1U CubeSat (10x10x10 cm), will be equipped with state-of-the-art solar cells provided by industry partners and will be launched into Low Earth Orbit (LEO). The mission focuses on measuring key parameters such as the IV curves, cell temperatures, and sun angles of the experimental solar cells mounted on the CubeSat's surfaces. Using an Aerospace Measurement Unit (AMU) and a low-bandwidth communication system, GauchoSat will transmit this data back to Earth for analysis. With sun-pointing capabilities and sensor integration, the project contributes to the future development of more efficient solar technologies in space exploration.

## Software Architecture
Our software is modular, designed around a hierarchical state machine using the QPNano framework. Key functionalities include:
- Managing CubeSat operational states and transitions.
- Interfacing with AMUs, IMUs, the radio and other peripherals.
- Ensuring robust communication with ground stations.

## Contributors

Brady Gin: Program Manager, Payload & Solar Lead

Nik Belle: Project Manager, Software Lead

Kpazawala Windross: Project Manager, Power & ADCS

Dakota Barnes: Program Manager, Radio/Comms Lead
