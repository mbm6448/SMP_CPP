//
// Created by moein on 8/2/2024.
//

#ifndef SEMI_MARKOV_PROCESS_H
#define SEMI_MARKOV_PROCESS_H

#include <vector>
#include <functional>
#include <memory>

// Class to handle the Gauss-Kronrod integration
class Integrator {
public:
    double adaptiveGaussKronrod(std::function<double(double)> f, double a, double b, double tol);

private:
    double gaussKronrod(std::function<double(double)> f, double a, double b, double &gaussResult);
};

// Class to represent a state transition with its CDF and transition probability
class Transition {
public:
    Transition(std::function<double(double)> cdf, double probability);

    double getProbability() const;
    double getCDF(double t) const;

private:
    std::function<double(double)> cdf_;
    double probability_;
};

// Class to represent the semi-Markov process
class SemiMarkovProcess {
public:
    SemiMarkovProcess(int numStates);

    void addTransition(int fromState, int toState, std::function<double(double)> cdf, double probability);
    double calculateTransitionProbability(int i, int j, double t, double tol);
    void setInitialProbabilities(const std::vector<std::vector<double>>& initialProbabilities);

private:
    int numStates_;
    Integrator integrator_;
    std::vector<std::vector<std::shared_ptr<Transition>>> transitions_;
    std::vector<std::vector<double>> initialProbabilities_;
};

#endif // SEMI_MARKOV_PROCESS_H


