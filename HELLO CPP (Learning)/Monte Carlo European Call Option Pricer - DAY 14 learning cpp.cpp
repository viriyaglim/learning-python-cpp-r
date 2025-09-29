#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <numeric>
#include <iomanip>
using namespace std;

// Monte Carlo European Call Option Pricer
double monte_carlo_call(double S0, double K, double r, double T,
                        double sigma, int nPaths, unsigned seed = 42) {
    mt19937 rng(seed);
    normal_distribution<double> norm(0.0, 1.0);

    double sumPayoff = 0.0;
    for (int i = 0; i < nPaths; i++) {
        double Z = norm(rng);
        double ST = S0 * exp((r - 0.5 * sigma * sigma) * T + sigma * sqrt(T) * Z);
        double payoff = max(ST - K, 0.0);
        sumPayoff += payoff;
    }

    return exp(-r * T) * (sumPayoff / nPaths);
}

// Monte Carlo with variance reduction (antithetic variates)
double monte_carlo_call_antithetic(double S0, double K, double r, double T,
                                   double sigma, int nPaths, unsigned seed = 42) {
    mt19937 rng(seed);
    normal_distribution<double> norm(0.0, 1.0);

    double sumPayoff = 0.0;
    for (int i = 0; i < nPaths / 2; i++) {
        double Z = norm(rng);
        // two paths: Z and -Z
        double ST1 = S0 * exp((r - 0.5 * sigma * sigma) * T + sigma * sqrt(T) * Z);
        double ST2 = S0 * exp((r - 0.5 * sigma * sigma) * T - sigma * sqrt(T) * Z);
        sumPayoff += max(ST1 - K, 0.0);
        sumPayoff += max(ST2 - K, 0.0);
    }

    return exp(-r * T) * (sumPayoff / nPaths);
}

int main() {
    cout << "=== C++ Quant Study Plan ===\n";
    cout << "[Day 14] Monte Carlo European Option Pricer (No Eigen)\n\n";
    cout << fixed << setprecision(6);

    // Parameters
    double S0 = 100.0;   // spot
    double K = 100.0;    // strike
    double r = 0.05;     // risk-free rate
    double T = 1.0;      // maturity (1 year)
    double sigma = 0.2;  // volatility
    int nPaths = 100000; // Monte Carlo paths

    double price_plain = monte_carlo_call(S0, K, r, T, sigma, nPaths);
    double price_av    = monte_carlo_call_antithetic(S0, K, r, T, sigma, nPaths);

    cout << "Monte Carlo Call (plain):       " << price_plain << "\n";
    cout << "Monte Carlo Call (antithetic): " << price_av << "\n";

    cout << "\n[Day 14] Done ✅\n";
    return 0;
}
