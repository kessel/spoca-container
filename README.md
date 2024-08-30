# SPoCA Container
This small repository makes the compilation and usage of the software SPoCA (https://github.com/bmampaey) easy, with containerization.

# Usage
The container image is build with docker. For that, install docker on your system and execute the script 
```bash
./build_docker.sh
```
This will clone the SPoCA repository and initiatlize the build process. The build process uses the docker base image `ubuntu:20.04`.

# The docker image
The docker image add the code of SPoCA to the directory /code and is compiled with `gcc`. Environment variables are so libraries are found. We provide a small script that makes running things in the container easy: `run_in_docker.sh` will mount your home directory and ensure you access files with our UID. You can use it as prefix to execute arbitrary commands 
```bash
./run_in_docker.sh my_favourite_command
```
or open a shell in the container
```bash
./run_in_docker.sh bash
```
# Convert docker image to apptainer
In order to convert the docker image to apptainer, we do the following steps:

1. Save the docker image as tar:
```bash
docker save spoca  -o spoca.tar
```
 2. (Optional) Copy the docker image to destination system. Preferable first clone this repository and copy the tar file to this very directory.

3. Convert to apptainer
```bash
build ./spoca.sif docker-archive://spoca.tar
``` 

# Scripts
For internal use, we have added scripts and corresponding configs that execute SPoCA in a defined way. The complexity is much lower than other automated workflows, but they can be a starting point for own developments.

