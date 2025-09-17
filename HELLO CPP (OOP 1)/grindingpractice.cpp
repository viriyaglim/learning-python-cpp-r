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

using namespace std;
using std::cout;
//how to use this file:
// input data
struct Param {
  int a;
  double b;
};

random_device rd;  // obtain a random number from hardware
mt19937 gen(rd()); // seed the generator
uniform_real_distribution<> distr(0, 10); // define the range


// output: we can return one or multiple
struct Result {
  int r;
};

int divide(int a, int b) {
  if (b == 0)
    throw logic_error("division by zero");
  return a / b;
}

// process
Result produce(Param p) { return {0}; }

// test
void test_produce() {
  //produce({.a = 0, .b = 3.0}).r == 0 ? true : throw logic_error("wrong result");
  // Or any problem-specific test
}

int test() {
  test_produce();
  return 0;
}

seed_seq seed{202050906};
auto mtgen = mt19937{seed};

int main() {

  test();

  {
    uniform_real_distribution<> uid(0, 10);
    cout << "seed: ";
    seed.param(std::ostream_iterator<int>(std::cout, " ")); // print the seed values
    cout << '\n';

    // save status of the generator to oss / restore from iss
    // alternatively, we can save to a file with ofstream/ifstream
    ostringstream oss;
    oss << mtgen;

    cout << uid(mtgen) << '\n';
    cout << uid(mtgen) << '\n';

    // restore the status of the generator
    istringstream iss{oss.str()};
    iss >> mtgen;

    cout << "same sequence after restore the sequence\n";
    cout << uid(mtgen) << '\n';
    cout << uid(mtgen) << '\n' << '\n';
  }

  cout << '\n';
  std::cout << "Hello World!\n";

  // Centralized definition
  using CONTAINER_TYPE = list<int>;
  using DATA_TYPE = double;
  using CONTAINER_TYPE_D = list<DATA_TYPE>;

  // Centralized definition
  /*
  using CONTAINER_TYPE = array<int, 1000>;
  using DATA_TYPE = double;
  using CONTAINER_TYPE_D = array<DATA_TYPE, 1000>;
  */

  CONTAINER_TYPE v(10);
  iota(v.begin(), v.end(), 0);
  for (const auto &x : v) {
    cout << x << ' ';
  }

  CONTAINER_TYPE_D vd(10);
  transform(v.begin(), v.end(), vd.begin(),
            [](auto x) { return static_cast<DATA_TYPE>(x) / 100; });
  
  for (const auto& x : vd) {
    cout << x << ' ';
  }

  CONTAINER_TYPE_D vdf;
  copy_if(vd.begin(), vd.end(), vdf.begin(), [](auto x) { return x < 0.4; });
  
  for (const auto& x : vdf) {
    cout << x << ' ';
  }

  cout << vdf.size() << '\n';

  // all_of, any_of, none_of
  all_of(vd.begin(), vd.end(), [](auto x) { return x < 0.5; }) ? cout << "all_of < 0.5\n" : cout << "not all_of < 0.5\n";
  any_of(vd.begin(), vd.end(), [](auto x) { return x < 0.5; }) ? cout << "any_of < 0.5\n" : cout << "not any_of < 0.5\n";
  none_of(vd.begin(), vd.end(), [](auto x) { return x < 0.5; }) ? cout << "none_of < 0.5\n" : cout << "not none_of < 0.5\n";
  //accumulate and reduce
  cout << "sum: " << accumulate(vd.begin(), vd.end(), 0.0) << '\n'; // difference between accumulate and reduce is that reduce can be parallelized
  cout << "sum: " << reduce(vd.begin(), vd.end(), 0.0) << '\n'; //accumulate starts from left to right, reduce can be parallelized
  //paralellized means that it can be computed in parallel, e.g. using multiple threads
  //reduce is commutative and associative, accumulate is not

  //for_each
  for_each(vd.begin(), vd.end(), [](auto &x) { x = x * 2; });
  for (const auto& x : vd) {
    cout << x << ' ';
  }
  cout << '\n';

  //reverse
  //set a new vector for the reversed
  vector<double> v_new(vd.rbegin(), vd.rend());
  for (const auto& x : v_new) {
    cout << x << ' ';
  }
  cout << '\n';

  //portfolio optimization example 
  // matrix multiplication A * B = C

  array<array<double, 3>, 3> A = {{{0.1, 0.2, 0.3}, {0.4, 0.5, 0.6}, {0.7, 0.8, 0.9}}};
  array<array<double, 2>, 3> B = {{{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}}};
  array<array<double, 2>, 3> C = {{{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}}};
  for (size_t i = 0; i < A.size(); ++i) {
    for (size_t j = 0; j < B[0].size(); ++j) {
      for (size_t k = 0; k < A[0].size(); ++k) {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
  for (const auto& row : C) {
    for (const auto& elem : row) {
      cout << elem << ' ';
    }
    cout << '\n';
  }
  cout << '\n';

  //taylor series expansion of lnx at x = 0
  double x = 0.5; // point to evaluate
  int n_terms = 100; // number of terms in the series
  double ln_x = 0.0;
  for (int n = 1; n <= n_terms; ++n) {
    double term = pow(-1, n + 1) * pow(x - 1, n) / n;
    ln_x += term;
  }
  cout << "ln(" << x << ") = " << ln_x << '\n';
  cout << "std::log(" << x << ") = " << std::log(x) << '\n';
  cout << "difference = " << fabs(ln_x - std::log(x)) << '\n';
  

  return 0;
}