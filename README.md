# Project Base of Vortex OpenSplice Usage

This repository shall provide an easy example to build upon. It contains a example IDL as given by OpenSplice and a simple CMakeLists.txt to generate everything.

## Requirements

* Windows: Visual Studio 2017 Professional
* Linux: GCC7
* CMake 

## Building

```bash
$> # Clone repository 
$> git clone https://github.com/tumbleowlee/opensplice-example
$> cd opensplice-example
$>
$> # Download the release of github.com/ADLINK-IST/opensplice/releases 
$> # and unzip/untar the zip/tar
$>
$> # Building 
$> mkdir build
$> cd build
$>
$> # Generate build files and build 
$> cmake -G "Visual Studio 15 2017 Win64" .. # Windows
$> cmake -G "Unix Makefiles" .. # Linux
$> cmake --build . --config Release
$>
$> # Execute publisher/subscriber
$> # OSPL_URI is needed for opensplice
$> set OSPL_URI=file://%cd%/../conf/ospl.xml # Windows
$> set OSPL_HOME=<PATH_TO_YOUR_OPENSPLICE_RELEASE> # Windows
$> export OSPL_URI=file://$(pwd)/../conf/ospl.xml # Linux
$> export OSPL_HOME=<PATH_TO_YOUR_OPENSPLICE_RELEASE> # Linux
$>
$> # All necessary opensplice libraries are copies into build after compilation
$> ./Release/publisher(.exe)
$> ./Release/subscriber(.exe)
```