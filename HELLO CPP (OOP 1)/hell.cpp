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
#include <iostream> 
#include <sstream>
#include <cmath>
#include <fstream> 
#include <cstdint>
#include <random> 
#include <chrono>
#include <functional> 

using namespace std;
using std::cout;
using std::endl;


void print_subset(const vector<int> &subset){
  for (auto x:subset) { cout << x << ","; }
}

void find_subsets(const vector<int> &S, vector <int> &subset, int index, int sum, int &count, bool debug){
  if (sum ==255) {
    if (debug) print_subset(subset);
    count++;
    return; 
    
  }
  if(index==S.size() || sum > 255)
    return; 
  subset.push_back(S[index]);
  find_subsets(S, subset, index+1, sum + S[index], count, debug);
  subset.pop_back(); 
  find_subsets(S, subset, index + 1, sum, count, debug);
}



//1. edited with the measuring time 
void q3_test_edited() {
  const vector<vector<int>> Ss{
    {1,3,5,11,13,21,31,33,35,37,41,43,51,53,55}, //167
    {255, 1,3,5,11,13,21,31,33,35,37,41,43,51,53,55}}; //168
  for (const auto &S:Ss) {
    vector<int> subset;
    int count = 0;
    auto start = chrono::high_resolution_clock::now(); //measuring time given debug false
    for (int i = 0; i < 100000; ++i) { //iteration 1e5 
      find_subsets(S, subset, 0,0, count, false);
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Total number of subsets with sum 255: " << count << "\n";
    cout << "Time taken for 100000 iterations: " << elapsed.count() << " seconds\n\n";
  }
}


//2. rewrite find_subsets as lambda function
void find_subsets_lambda(const vector<int> &S, vector<int> &subset, int index, int sum, int &count, bool debug) {
  if (sum == 255) {
    if (debug) print_subset(subset);
    count++;
    return;
  }
  if (index == S.size() || sum > 255)
    return;
  subset.push_back(S[index]);
  
  // Lambda function for recursive calls
  auto recursive_call = [&](int new_index, int new_sum) {
    find_subsets_lambda(S, subset, new_index, new_sum, count, debug);
  };
  
  recursive_call(index + 1, sum + S[index]); // Include current element
  subset.pop_back();
  recursive_call(index + 1, sum); // Exclude current element
}



// 3. find ways to optimze find_subsets()  for speed (not using inline) with a new function find_subsets_opt() and measure its running time, add comments in the code to explain your optimization. 
