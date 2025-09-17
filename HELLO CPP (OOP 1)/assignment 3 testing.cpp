#include <iostream>
#include <iomanip>
#include <vector>
#include <tuple>
#include <string>
#include <cmath>
#include <cassert>
#include <functional>
#include <random>
#include <chrono>

using namespace std;
using std::cout;

//q3_1 
//Write a program to print a table of values for the function 
double f(double x) {
    return pow(sin(x),2) + exp(-pow(x,2)) + pow(x,3) * log(x+1);
}

//q3_3
// Write and test a function:
vector<double> rand_vector(int n, double a=0.0, double b = 1.0){
    vector<double> v(n);
    random_device rd; //given
    mt19937 gen(rd());
    uniform_real_distribution<double> uni(a,b);
    for (int i=0; i<n; i++){
        v[i] = uni(gen); //generate random number
    }
    return v;
}

 
//q3_4
//Part 1: Write a function void change_x(int x) that attempts to increase the value of x by 1.
void change_x(int x){ x = x + 1; }

// Part 2: Write a function void change_x_with_ref(int& x) that increases the value of x by 1.
void change_x_with_ref(int& x){ x = x + 1;}

// Part 3: Write a function void value_copy(std::vector<int> v) that does nothing and call it 1000 times in the main function with a vector of size 1 million as input. 
void value_copy(std::vector<int> v){} // Measure and note the program execution time.

// Part 4: Create a new function void value_copy_with_ref(const std::vector<int>& v) to do the same task as value_copy, and measure the running time.
void value_copy_with_ref(const std::vector<int>& v){}

//q3_7
using RateCurve = vector<tuple<int,double>>;

// Setup for Interpolation
double interpolate(const vector<int>& dates, const vector<double>& rates, int date) {
    if (date <= dates.front()) return rates.front();   // flat extrapolation (left)
    if (date >= dates.back())  return rates.back();    // flat extrapolation (right)

    for (size_t i = 0; i + 1 < dates.size(); ++i) {
        int d1 = dates[i], d2 = dates[i+1];
        double r1 = rates[i], r2 = rates[i+1];
        if (date >= d1 && date <= d2) {
            return r1 + (r2 - r1) * double(date - d1) / double(d2 - d1);
        }
    }
    return rates.back();
}


int main(){
    //q3_1
    cout << "This is for question 3-1\n";
    for (double x = 0.0; x<=1.0; x+=0.1){
        cout << "x=" << setw(5) << fixed << setprecision(1) << x << "   " << "f(x)=" << setw(8) << fixed << setprecision(5) << f(x) <<  endl;
    }

    //q3_3
    cout << "This is for question 3-3\n";
    cout << fixed << setprecision(5);
    bool verify_in_range = true;
    // Test range 1
    auto v = rand_vector(6); // given default a = 0.0, b = 1.0
    cout << "Random vector with 6 elements in range [0.0,1.0]:\n";
    for (double x: v) {
        cout << x << " ";         
        if (x < 0.0 || x > 1.0){
        verify_in_range = false;
        }
        
    }
    cout << endl;

    // Test range 2
    auto v1 = rand_vector(7,-1.0,1.0); // a = -1.0, b = 1.0
    cout << "Random vector with 7 elements in range [-1.0,1.0]:\n";
    for (double x: v1) {
        cout << x << " ";         
        if (x < -1.0 || x > 1.0){
        verify_in_range = false;
        }
    }
    cout << endl;

    // Test range 3
    auto v2 = rand_vector(8,10.0,20.0); // a = 10.0, b = 20.0
    cout << "Random vector with 8 elements in range [10.0,20.0]:\n";
    for (double x: v2) {
        cout << x << " ";         
        if (x < 10.0 || x > 20.0){
        verify_in_range = false;
        }
    }
    cout << endl;

    if (verify_in_range==true) //verify
        cout << "All numbers are within desired range." << endl;
    else
        cout << "Some numbers are out of range!" << endl;



    //q3_4
    cout << "This is for question 3-4\n";
    int y = 7;
    cout << "Initial value y is " << y << endl;

    //Part 1 and 2
    change_x(y); //  Verify that a function call change_x(y) has no effect on the value of an int variable y.
    cout << "After change_x(y), y is " << y << endl;

    change_x_with_ref(y); // Verify that a function call change_x_with_ref(y) indeed changes the value of y.
    cout << "After change_x_with_ref(y), y is " << y << endl;

    cout << "Function call change_x(y) has no effect, while change_x_with_ref(y) changes the value of y by increasing 1." << endl;

    // Part 3 and 4
    vector<int> vec(1000000,1);
    auto start = chrono::high_resolution_clock::now();
    for(int i = 0; i<1000; i++){
        value_copy(vec);
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end-start);

    auto start1 = chrono::high_resolution_clock::now();
    for(int i = 0; i<1000; i++){
        value_copy_with_ref(vec);
    }
    auto end1 = chrono::high_resolution_clock::now();
    auto duration1 = chrono::duration_cast<chrono::milliseconds>(end1-start1);

    cout << "Time taken for value_copy: " << duration.count() << " milliseconds:" << endl;
    cout << "Time taken for value_copy_with_ref: " << duration1.count() << " milliseconds:" << endl;
    cout << "value_copy_with_ref is faster than value_copy" << endl; 


    //q3_7
    //Given 
    RateCurve IC{
        {41082, 0.0},
        {41083, 0.3447205943771747},
        {41085, 0.3447205943771747},
        {41092, 0.368367772605730},
        {41116, 0.525570772180778},
        {41146, 0.589858326742378}
    };

    //Based on Idea 1
    vector<int> dates;
    vector<double> rates;
    for (size_t i = 0; i < IC.size(); ++i) {
        auto [d, r] = IC[i];
        dates.push_back(d);
        rates.push_back(r);
    }



    // Quick tests
    cout << fixed;
    cout << "date=41080 (left extrapolation): " << interpolate(dates, rates, 41080) << "\n";
    cout << "date=41083 (exact grid point):   " << interpolate(dates, rates, 41083) << "\n";
    cout << "date=41088 (linear between):     " << interpolate(dates, rates, 41088) << "\n";
    cout << "date=41150 (right extrapolation):" << interpolate(dates, rates, 41150) << "\n";

    
    return 0;


}