# Simulation environment

Download the Renesas MCU Rally Virtual environment from the Git repository. You can download the .zip file or, if you are familiar with git, clone the repository.

- Option 1: Download and unpack the .zip file: [link](https://github.com/ul-lakos/renesas_mcu_rally/archive/master.zip)

- Option 2: Clone from the git repository:

``` bash
git clone https://github.com/ul-lakos/renesas_mcu_rally.git
```

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
