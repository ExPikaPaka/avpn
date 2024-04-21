# VPN server

Welcome to our Server! This README will guide you through the steps to build and run the app using Makefile.

## Build Instructions

1. Ensure you have `g++` installed on your system.

2. Clone this repository to your local machine.

3. Open a terminal and navigate to the directory where you cloned the repository.

4. Run the following command to build the server: 

make

## Running the Server

Once the server is built, you can run it using the following command:

./Server <port> <num_threads>

Example: ./Server 1194 4

## Cleaning Up

To clean up the compiled object files and executable, you can use the following command:

make clean