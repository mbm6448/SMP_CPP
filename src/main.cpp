#include "smp.h"
#include "tinyxml2.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <omp.h>

using namespace tinyxml2;

void parseCSV(const std::string &filename, std::vector<std::tuple<int, int, double>> &transitions) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open CSV file.");
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        int i, j;
        double probability;
        char comma;
        ss >> i >> comma >> j >> comma >> probability;
        transitions.emplace_back(i, j, probability);
    }

    file.close();
}

void parseCSV(const std::string &filename, std::vector<std::vector<double>> &initialProbabilities) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open CSV file.");
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::vector<double> row;
        std::string value;
        while (std::getline(ss, value, ',')) {
            row.push_back(std::stod(value));
        }
        initialProbabilities.push_back(row);
    }

    file.close();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config.xml>\n";
        return 1;
    }

    std::string xmlFile = argv[1];

    // Parse the XML file
    XMLDocument doc;
    if (doc.LoadFile(xmlFile.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load XML file.\n";
        return 1;
    }

    XMLElement* root = doc.FirstChildElement("configuration");
    if (!root) {
        std::cerr << "Invalid XML file format.\n";
        return 1;
    }

    // Read parameters from XML
    int numStates = root->FirstChildElement("states")->IntText();
    double t_max = root->FirstChildElement("t_max")->DoubleText();
    double time_step = root->FirstChildElement("time_step")->DoubleText();
    double tol = root->FirstChildElement("tol")->DoubleText();
    std::string transitionFile = root->FirstChildElement("transition_file")->GetText();
    std::string initialFile = root->FirstChildElement("initial_file")->GetText();

    // Prepare to parse the CSV files
    std::vector<std::tuple<int, int, double>> transitions;
    std::vector<std::vector<double>> initialProbabilities;

    try {
        parseCSV(transitionFile, transitions);
        parseCSV(initialFile, initialProbabilities);
    } catch (const std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

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

    return 0;
}

