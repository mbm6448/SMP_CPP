#include "ConfigParser.h"
#include "tinyxml2.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

ConfigParser::ConfigParser(const std::string &filename) {
    if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS) {
        throw std::runtime_error("Failed to load XML file.");
    }

    root = doc.FirstChildElement("configuration");
    if (!root) {
        throw std::runtime_error("Invalid XML file format.");
    }
}

int ConfigParser::getNumStates() const {
    return root->FirstChildElement("states")->IntText();
}

double ConfigParser::getTMax() const {
    return root->FirstChildElement("t_max")->DoubleText();
}

double ConfigParser::getTimeStep() const {
    return root->FirstChildElement("time_step")->DoubleText();
}

double ConfigParser::getTolerance() const {
    return root->FirstChildElement("tol")->DoubleText();
}

std::string ConfigParser::getTransitionFile() const {
    return root->FirstChildElement("transition_file")->GetText();
}

std::string ConfigParser::getInitialFile() const {
    return root->FirstChildElement("initial_file")->GetText();
}

void ConfigParser::parseCSV(const std::string &filename, std::vector<std::tuple<int, int, double>> &transitions) const {
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

void ConfigParser::parseCSV(const std::string &filename, std::vector<std::vector<double>> &initialProbabilities) const {
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
