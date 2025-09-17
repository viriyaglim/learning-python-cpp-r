#include <algorithm>
#include <numeric>
#include <vector> 
#include <list>
#include <string>
#include <tuple>
#include <set>
#include <array> 
#include <iterator>
#include <random>
#include <array>
#include <iostream> 
#include <sstream>
#include <cmath>
#include <fstream> 
#include <cstdint>
#include <random> 
#include <unordered_map>

using namespace std;
using std::cout;
using std::endl;

//a user can send a certain amount of money to a few users, the recipients will be allocated for the amount one by one in order. the amount money allocated to be uniform randomly chosen from the interval [0.01, remaining averaged per person *2 ]

// 50 was split into [14.34, 8.12, 5.67, 11.35, 10.52]
// 1. write a c++ function to implement above scheme function should return the amount each one receives, implement t in function send(size_t, double amount), amount shall be rounded to 2 dec points, using round from cmath, return each person amount receives, implement send(size t n) and test it with void test_send(0 for the 5 people and 10 people case wth following print out 5: xx.xx, xx.xx, xx.xx, xx.xx, xx.xx,
// 10: xx.xx, xx.xx, xx.xx, xx.xx, xx.xx, xx.xx, xx.xx, xx.xx, xx.xx, xx.xx

vector<double> send(size_t n, double amount) {
    vector<double> result(n, 0.0);
    random_device rd; // obtain a random number from hardware
    mt19937 gen(rd()); // seed the generator

    double remaining = amount;
    for (size_t i = 0; i < n; ++i) {
        if (i == n - 1) {
            result[i] = round(remaining * 100) / 100; // last person gets the rest
        } else {
            double avg = remaining / (n - i);
            uniform_real_distribution<> dis(0.01, avg * 2); // define the range
            double allocated = dis(gen);
            allocated = round(allocated * 100) / 100; // round to 2 decimal points
            result[i] = allocated;
            remaining -= allocated;
        }
    }
    return result;
}
void test_send() {
    for (size_t n : {5, 10}) {
        double amount = 50.0;
        vector<double> allocations = send(n, amount);
        cout << n << ": ";
        for (size_t i = 0; i < allocations.size(); ++i) {
            cout << allocations[i];
            if (i != allocations.size() - 1) cout << ", ";
        }
        cout << endl;
    }
}

//2. perform simulation of 1e5 times for 5 people and 10 people, calculate probabibility when the first one reveives more than last one, implement it in function void calc_first_vs_last(size_t n) with 5: 0.xxx 10: 0.xxx 
void calc_first_vs_last(size_t n) {
    size_t count_first_greater = 0;
    size_t simulations = 100000;

    for (size_t i = 0; i < simulations; ++i) {
        vector<double> allocations = send(n, 50.0);
        if (allocations[0] > allocations[n - 1]) {
            count_first_greater++;
        }
    }

    double probability = static_cast<double>(count_first_greater) / simulations;
    cout << n << ": " << probability << endl;
}




//3. perform simulation of 1e5 times for 5 people and 10 people, calculate probabibility for each one's probability to get largest among all, implement it in function void calc_each_prob(size_t n) with 5: 0.xxx, 0.xxx, 0.xxx, ....

void calc_each_prob(size_t n) {
    vector<size_t> count_largest(n, 0);
    size_t simulations = 100000;

    for (size_t i = 0; i < simulations; ++i) {
        vector<double> allocations = send(n, 50.0);
        size_t index_of_largest = distance(allocations.begin(), max_element(allocations.begin(), allocations.end()));
        count_largest[index_of_largest]++;
    }

    cout << n << ": ";
    for (size_t i = 0; i < n; ++i) {
        double probability = static_cast<double>(count_largest[i]) / simulations;
        cout << probability;
        if (i != n - 1) cout << ", ";
    }
    cout << endl;
}

//compile to void q4()
void q4() {
    test_send();
    calc_first_vs_last(5);
    calc_first_vs_last(10);
    calc_each_prob(5);
    calc_each_prob(10);
}