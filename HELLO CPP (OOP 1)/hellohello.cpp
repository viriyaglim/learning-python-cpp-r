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

  // Create an integer array perm of length 100 with entries 0,1,...,99 in this order. 
void q1() {  
  array<int, 100> perm;
    iota(perm.begin(), perm.end(), 0); // fill with 0, 1, ..., 99
    random_device rd; // obtain a random number from hardware
    mt19937 gen(rd()); // seed the generator
    for (int i = 99; i > 0; --i) {
        uniform_int_distribution<> dis(0, i); // define the range
        int j = dis(gen); // generate a random index j in [0, i]
        swap(perm[i], perm[j]); // swap entries perm[i] and perm[j]
    }
    return perm;
}   

  //write a function that generates a random permutation of 0,1,..,99 and print them to screen in comma-separated format
  vector<int> generate_permutation() {
      array<int, 100> perm;
      iota(perm.begin(), perm.end(), 0); // fill with 0, 1, ..., 99
      random_device rd; // obtain a random number from hardware
      mt19937 gen(rd()); // seed the generator
      for (int i = 99; i > 0; --i) {
          uniform_int_distribution<> dis(0, i); // define the range
          int j = dis(gen); // generate a random index j in [0, i]
          swap(perm[i], perm[j]); // swap entries perm[i] and perm[j]
      }
      return vector<int>(perm.begin(), perm.end());
  }

  //write a test function that generate 1000 permutation using the function above and count the number of unique occurence of each number on each index and print the result on screen
void test_permutations() {
      vector<unordered_map<int, int>> count(100);
      for (int i = 0; i < 1000; ++i) {
          auto perm = generate_permutation();
          // Print or validate the permutation
          for (int j = 0; j < perm.size(); ++j) {
              count[j][perm[j]]++;
          }
      }
      // Print the results
      for (int i = 0; i < count.size(); ++i) {
          cout << "Index " << i << ":\n";
          for (const auto& [num, freq] : count[i]) {
              cout << "  Number " << num << ": " << freq << " times\n";
          }
      }
  }

          }
          cout << '\n'; // Print a newline after each permutation
      }
  }

int main() {
  q1();
  return 0;
}