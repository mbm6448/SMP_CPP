#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include "tinyxml2.h"
#include <string>
#include <vector>
#include <tuple>

class ConfigParser {
public:
    ConfigParser(const std::string &filename);

    int getNumStates() const;
    double getTMax() const;
    double getTimeStep() const;
    double getTolerance() const;
    std::string getTransitionFile() const;
    std::string getInitialFile() const;

    void parseCSV(const std::string &filename, std::vector<std::tuple<int, int, double>> &transitions) const;
    void parseCSV(const std::string &filename, std::vector<std::vector<double>> &initialProbabilities) const;

private:
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root;
};

#endif // CONFIG_PARSER_H
