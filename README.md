# Automatic Guided Vehicle Demo

This project demonstrates the use of [RAI](https://github.com/RobotecAI/rai) framework paired with [O3DE](https://www.o3de.org/) game engine for the agricultural simulation. In particular, it shows the communication between Generative AI models and tractors navigating between trees in the orchard.

## Demo description

The demonstration comprises a simulation of two autonomous tractors navigating along predefined trajectories within an orchard environment. Various obstacles are placed between tree rows, causing the tractors to stop and transmit requests for assistance via ROS 2 messaging protocols. The RAI framework intercepts these messages. Next, it integrates them with sensor and contextual information to subsequently trigger a predetermined action for each tractor based on the Generative's AI analysis.

There are three obstacles in total, including a compost bag, a set of barrels, and fallen tree branches. There is also one false positive detection, i.e., the tractor stops and asks for help with no reason. Each tractor can continue cruising through the obstacle, reverse and replan the path or use a signal (light and sound).

### Screenshots

![Screenshot0](docs/images/demo0.png)
Birdview of the orchard with predefined trajectories of two tractors.


![Screenshot0](docs/images/demo1.png)
A tractor reaching one of the obstacles (a compost bag and a spade).


![Screenshot0](docs/images/demo2.png)
A tractor reaching one of the obstacles; view from the interior (a set of barrels).

### Sample conversation

The sample conversation between the AI Assistant and the tractor orchestrated by RAI framework can be found [here](docs/sample.md).

## O3DE Demo Description

The demo is provided as a binary package for Ubuntu 24.04 with ROS 2 Jazzy and Ubuntu 22.04 with ROS 2 Humble, ensuring quick and easy setup. The binary package is the recommended method to run the demo. The source code is also available for those who prefer to build the project themselves.

The project was tested on Ubuntu 22.04 with ROS 2 Humble and Ubuntu 24.04 with ROS 2 Jazzy.

> **Note:** This demo is **not supported on Windows!**

### Hardware
* [Hardware requirements of O3DE](https://www.o3de.org/docs/welcome-guide/requirements/)
* Ubuntu 22.04 (Jammy) with ROS 2 Humble or Ubuntu 24.04 (Noble) with ROS 2 Jazzy
* At least 60 GB of free disk space

### Building the demo from the source

Please follow the instructions below to build the project. The instructions are based on a common base folder: $DEMO_BASE (absolute path). Install [ROS 2 first](https://docs.ros.org/en/humble/Installation/Ubuntu-Install-Debians.html) and `git-lfs` package to pull the binary files. Install all `ROS 2` packages that are [required by O3DE](https://docs.o3de.org/docs/user-guide/interactivity/robotics/project-configuration/).

1. Install `git-lfs` package and pull the codebase.
```bash
sudo apt-get install git-lfs
cd $DEMO_BASE
git clone https://github.com/RobotecAI/rai-agriculture-demo.git
```

2. Clone O3DE and register the engine

```bash
cd $DEMO_BASE
git clone https://github.com/o3de/o3de.git -b main
cd $DEMO_BASE/o3de
git lfs install
git lfs pull
python/get_python.sh
scripts/o3de.sh register --this-engine
```

3. Clone and register the ROS2 Gem locally; register the RosRobotSample Gem locally

```bash
cd $DEMO_BASE
git clone https://github.com/o3de/o3de-extras.git -b main
$DEMO_BASE/o3de/scripts/o3de.sh register -gp $DEMO_BASE/o3de-extras/Gems/ROS2
$DEMO_BASE/o3de/scripts/o3de.sh register -gp $DEMO_BASE/o3de-extras/Gems/RosRobotSample
```

4. Clone gems from other repositories that are required for this demo

```bash
cd $DEMO_BASE
git clone https://github.com/RobotecAI/robotec-o3de-tools.git
$DEMO_BASE/o3de/scripts/o3de.sh register -gp $DEMO_BASE/robotec-o3de-tools/Gems/CsvSpawner
$DEMO_BASE/o3de/scripts/o3de.sh register -gp $DEMO_BASE/robotec-o3de-tools/Gems/ROS2ScriptIntegration
git clone https://github.com/RobotecAI/robotec-agriculture-assets.git
$DEMO_BASE/o3de/scripts/o3de.sh register -agp $DEMO_BASE/robotec-agriculture-assets/
git clone https://github.com/RobotecAI/robotec-generic-assets.git
$DEMO_BASE/o3de/scripts/o3de.sh register -agp $DEMO_BASE/robotec-generic-assets/RobotecSmallContainersAssets
$DEMO_BASE/o3de/scripts/o3de.sh register -agp $DEMO_BASE/robotec-generic-assets/RobotecSkyboxAssets
```

## Building

You can prepare the simulation executable with:
- development build, or
- exporting the project.

**Development build** is tied with the local assets and is focused on development. It also includes the _Editor_, which can ue used to modify the scene before running the simulation.

**Exporting the project** creates a bundled and portable version of the simulation that can be moved between PCs.

## Development build

1. Build development build

```bash
cd $DEMO_BASE/rai-agriculture-demo/Project
cmake -B build/linux -G "Ninja Multi-Config" -DLY_STRIP_DEBUG_SYMBOLS=TRUE -DLY_DISABLE_TEST_MODULES=ON
cmake --build build/linux --config profile --target RAIAgricultureDemo.Assets RAIAgricultureDemo.GameLauncher Editor
```

2. Run simulation

```bash
$DEMO_BASE/rai-agriculture-demo/Project/build/linux/bin/profile/RAIAgricultureDemo.GameLauncher -bg_ConnectToAssetProcessor=0
```

## Exporting project

1. Export the project using the o3de export tool
```bash
$DEMO_BASE/o3de/scripts/o3de.sh export-project -es ExportScripts/export_source_built_project.py --project-path $DEMO_BASE/rai-agriculture-demo/Project -assets --fail-on-asset-errors -noserver -out $DEMO_BASE/rai-agriculture-demo/release --build-tools --seedlist $DEMO_BASE/rai-agriculture-demo/Project/AssetBundling/SeedLists/husarion.seed --no-unified-launcher
```

2. Run the simulation
```bash
$DEMO_BASE/rai-agriculture-demo/release/RAIAgricultureDemoGamePackage/RAIAgricultureDemo.GameLauncher
```