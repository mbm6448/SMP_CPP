# Semi-Markov Process Simulation

This project implements a Semi-Markov Process (SMP) simulation in C++. The simulation allows for flexible configuration of the number of states, transition probabilities, and initial probabilities through XML and CSV files. The code is parallelized using OpenMP to efficiently compute transition probabilities over time.

## Features

- **Configurable Parameters**: Specify the number of states, transition probabilities, and initial probabilities via XML and CSV files.
- **OpenMP Parallelization**: The simulation leverages OpenMP to perform calculations in parallel, speeding up computation.
- **Numerical Integration by Gauss-Kronod method**: To get the transition probability matrix at each time step. 
- **XML and CSV Integration**: Use an XML file to point to CSV files containing necessary data, making the simulation flexible and easy to configure.

## Project Structure

```
project_root/
│
├── include/
│   └── smp.h                  # Header file for the Semi-Markov Process class
├── src/
│   ├── main.cpp               # Main program that runs the simulation
│   └── smp.cpp           # Implementation of the Semi-Markov Process class
├── config.xml                 # Sample configuration file
├── transitions.csv            # Sample CSV file with transition data
├── initial_probabilities.csv  # Sample CSV file with initial probabilities
└── Makefile                   # Makefile to build the project
```

## Prerequisites

- **C++ Compiler**: Ensure you have `g++` installed and supports at least C++17 standard.
- **TinyXML2**: Install TinyXML2 via your package manager or include the source files in your project.
- **OpenMP**: Make sure your compiler supports OpenMP for parallel execution.

## Installation

### Install Dependencies

For Ubuntu/Debian:

```bash
sudo apt update
sudo apt install g++ libtinyxml2-dev
```

For macOS (using Homebrew):

```bash
brew install tinyxml2
```

### Build the Project

Clone the repository and navigate to the project directory:

```bash
git clone https://github.com/mbm6448/SMP_CPP.git
cd SMP_CPP
```

Compile the project using the provided `Makefile`:

```bash
make
```

This will generate an executable in the `bin/` directory.

## Usage

1. **Configure the Simulation**: Modify the `config.xml` file to specify the number of states, the maximum time, time step, tolerance, and paths to the CSV files containing the transition and initial probabilities.

2. **Run the Simulation**:

   ```bash
   ./bin/semi_markov config.xml
   ```

3. **Check the Output**: The simulation results will be written to `transition_probabilities.csv` in the project root.

## Configuration Files

### `config.xml`

This file specifies the configuration for the simulation, including the number of states and the paths to the CSV files for transition and initial probabilities.

```xml
<configuration>
    <states>3</states>
    <t_max>10.0</t_max>
    <time_step>1.0</time_step>
    <tol>1e-6</tol>
    <transition_file>transitions.csv</transition_file>
    <initial_file>initial_probabilities.csv</initial_file>
</configuration>
```

### `transitions.csv`

This CSV file specifies the transition probabilities between states.

```csv
#transitions
0,1,0.4
0,2,0.6
1,2,1.0
```

### `initial_probabilities.csv`

This CSV file specifies the initial probabilities for each state.

```csv
1.0,0.0,0.0
0.0,1.0,0.0
0.0,0.0,1.0
```

