#include "ConfigParser.h"
#include "smp.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config.xml>\n";
        return 1;
    }

    try {
        ConfigParser config(argv[1]);

        int numStates = config.getNumStates();
        double t_max = config.getTMax();
        double time_step = config.getTimeStep();
        double tol = config.getTolerance();
        std::string transitionFile = config.getTransitionFile();
        std::string initialFile = config.getInitialFile();

        // Prepare to parse the CSV files
        std::vector<std::tuple<int, int, double>> transitions;
        std::vector<std::vector<double>> initialProbabilities;

        config.parseCSV(transitionFile, transitions);
        config.parseCSV(initialFile, initialProbabilities);

        int numTimeSteps = static_cast<int>(t_max / time_step);  // Convert to integer for loop control

        // Create a semi-Markov process with the specified number of states
        SemiMarkovProcess smp(numStates);

        // Add transitions from the parsed CSV file
        for (const auto& [i, j, probability] : transitions) {
            smp.addTransition(i, j, [probability](double t) {
                return 1.0 - exp(-probability * t);  // exponential CDF
            }, probability);
        }

        // Set initial probabilities
        smp.setInitialProbabilities(initialProbabilities);

        // Open a CSV file for writing the results
        std::ofstream outputFile("transition_probabilities.csv");
        if (!outputFile.is_open()) {
            std::cerr << "Error: Could not open file for writing." << std::endl;
            return 1;
        }

        // Write the header row
        outputFile << "Time,State_i,State_j,P_ij(t)\n";

        // Loop through each time step
        for (int timeIndex = 0; timeIndex <= numTimeSteps; ++timeIndex) {
            double t = timeIndex * time_step;
            std::vector<std::string> results; // Store results for this time step

            // Parallel region using OpenMP
            #pragma omp parallel
            {
                std::vector<std::string> localResults; // Store local results for each thread

                // Loop through state pairs
                #pragma omp for schedule(dynamic)
                for (int i = 0; i < numStates; ++i) {
                    for (int j = 0; j < numStates; ++j) {
                        if (i != j) {
                            double probability = smp.calculateTransitionProbability(i, j, t, tol);
                            if (probability > 0.0) { // Only write positive probabilities
                                localResults.push_back(std::to_string(t) + "," + std::to_string(i) + "," + std::to_string(j) + "," + std::to_string(probability) + "\n");
                            }
                        }
                    }
                }

                // Critical section to combine results from all threads
                #pragma omp critical
                {
                    results.insert(results.end(), localResults.begin(), localResults.end());
                }
            }

            // Write the combined results for this time step to the file
            for (const auto &line : results) {
                outputFile << line;
            }
        }

        // Close the file
        outputFile.close();
        std::cout << "Transition probabilities have been written to transition_probabilities.csv" << std::endl;

    } catch (const std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
