# Simulation environment

Download the Renesas Virtual MCU Rally environment from the Git repository:

- Navigate to [Latest releases](https://github.com/renesas-virtual-mcu-rally/simulator/releases/)
- Download the .zip file
  - Windows users: renesas_virtual_mcu_rally_windows
  - Linux users: renesas_virtual_mcu_rally_linux
  - Advanced users: clone the repository instead, and compile the controllers yourself

Run the demo:

- Open WeBots, select File -> Open World...
- Navigate to the Simulation environment folder obtained during the installation.
- Open: embedded_world.wbt from the worlds folder

Explore the structure of the project:

- controllers: your program will go here
  - referee: the automated refereeing program used by the timing gate asset
  - safety_car_controller: a simple robot controller, used by the safety car
- docs: these instructions, manuals, etc.
- libraries: includes the robot programming API, used by your program
- plugins: empty, but can contain additional plugins for WeBots
- protos: assets, such as the robot model, the timing gate, etc.
  - track parts: assets for building custom tracks
- worlds: contains world files, main project files that WeBots can open
  - 3dmodels: 3D models used by the project
  - textures: textures used by the project
