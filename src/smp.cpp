#include "smp.h"
#include <cmath>
#include <iostream>

// Implementation of Integrator methods
double Integrator::adaptiveGaussKronrod(std::function<double(double)> f, double a, double b, double tol) {
    double gaussResult;
    double kronrodResult = gaussKronrod(f, a, b, gaussResult);
    double errorEstimate = std::abs(kronrodResult - gaussResult);

    if (errorEstimate < tol) {
        return kronrodResult;
    } else {
        double mid = 0.5 * (a + b);
        double leftResult = adaptiveGaussKronrod(f, a, mid, tol / 2);
        double rightResult = adaptiveGaussKronrod(f, mid, b, tol / 2);
        return leftResult + rightResult;
    }
}

double Integrator::gaussKronrod(std::function<double(double)> f, double a, double b, double &gaussResult) {
    // Gauss-Kronrod nodes and weights (10-point)
    std::vector<double> nodes = {0.0, -0.1834346424956498, 0.1834346424956498, -0.5255324099163290, 0.5255324099163290,
                                 -0.7966664774136267, 0.7966664774136267, -0.9602898564975363, 0.9602898564975363};

    std::vector<double> gaussWeights = {0.3626837833783620, 0.3626837833783620, 0.3137066458778873, 0.3137066458778873,
                                        0.2223810344533745, 0.2223810344533745, 0.1012285362903763, 0.1012285362903763};

    std::vector<double> kronrodWeights = {0.3302393550012598, 0.1806481606948574, 0.1806481606948574, 0.0812743883615744,
                                          0.0812743883615744, 0.3123470770400029, 0.3123470770400029, 0.2606106964029354,
                                          0.2606106964029354};

    double mid = 0.5 * (b + a);
    double halfLength = 0.5 * (b - a);

    double integralGauss = 0.0;
    double integralKronrod = 0.0;

    for (size_t i = 0; i < nodes.size(); ++i) {
        double node = halfLength * nodes[i];
        integralKronrod += kronrodWeights[i] * f(mid + node);
        if (i < gaussWeights.size()) {
            integralGauss += gaussWeights[i] * f(mid + node);
        }
    }

    integralKronrod *= halfLength;
    integralGauss *= halfLength;

    gaussResult = integralGauss;
    return integralKronrod;
}

// Implementation of Transition methods
Transition::Transition(std::function<double(double)> cdf, double probability)
        : cdf_(cdf), probability_(probability) {}

double Transition::getProbability() const {
    return probability_;
}

double Transition::getCDF(double t) const {
    if (cdf_) {
        return cdf_(t);
    } else {
        return 0.0;
    }
}

// Implementation of SemiMarkovProcess methods
SemiMarkovProcess::SemiMarkovProcess(int numStates)
        : numStates_(numStates), transitions_(numStates, std::vector<std::shared_ptr<Transition>>(numStates, nullptr)) {}

void SemiMarkovProcess::addTransition(int fromState, int toState, std::function<double(double)> cdf, double probability) {
    transitions_[fromState][toState] = std::make_shared<Transition>(cdf, probability);
}

double SemiMarkovProcess::calculateTransitionProbability(int i, int j, double t, double tol) {
    auto integrand = [&](double u) -> double {
        double sum = 0.0;
        for (int k = 0; k < numStates_; ++k) {
            if (transitions_[k][j]) {
                sum += initialProbabilities_[i][k] * transitions_[k][j]->getProbability() * transitions_[k][j]->getCDF(t - u);
            }
        }
        return sum;
    };

    // Calculate the raw transition probability using integration
    double rawProbability = integrator_.adaptiveGaussKronrod(integrand, 0.0, t, tol);

    // Normalize the raw probability if it's greater than 1
    return (rawProbability > 1.0) ? 1.0 : rawProbability;

}

void SemiMarkovProcess::setInitialProbabilities(const std::vector<std::vector<double>>& initialProbabilities) {
    initialProbabilities_ = initialProbabilities;
}

