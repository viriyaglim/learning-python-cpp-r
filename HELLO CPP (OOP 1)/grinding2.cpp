#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cctype>     
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <numbers>
#include <numeric>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>
#include <chrono>
#include <typeinfo>
#include <cassert>

using namespace std;
using std::cout;

//question 4-1
int gcd(int a, int b){
    if (a==0) return b;
    if (b==0) return a;
    while (b != 0) {
        int r = b; // remainder
        b = a % b; // update b to a mod b
        a = r; // update a to previous b
    }
    return a; // gcd is now in a
}

//question 4-2
// normal version
long long P(int n, int k) { // number of partitions of n using parts up to k
  if (k == 1)
    return 1;
  if (n == 0)
    return 1;
  if (n < 0 || k < 1)
    return 0;
  return P(n, k - 1) + P(n - k, k);
}

long long p(int n) { return P(n, n); }

// ChatGPT helped me to simplify the memoization version, I'm still trying to understand it. 
// memoization version 
// using a 2D vector to store computed values
long long P_memo(int n, int k, std::vector<std::vector<long long>> &memo) {
  if (k == 1)
    return 1;
  if (n == 0)
    return 1;
  if (n < 0 || k < 1)
    return 0;
  long long &hello = memo[n][k]; // reference to the memoized value
  if (hello != -1) // if already computed
    return hello;
  hello = P_memo(n, k - 1, memo) + P_memo(n - k, k, memo); // compute and store
  return hello; // store and return the computed value
}

long long p_memo(int n) {
  std::vector<std::vector<long long>> memo(n + 1,
                                           std::vector<long long>(n + 1, -1));
  return P_memo(n, n, memo);
}

//assignment 4
// Question 4-3
void bubble_sort(std::vector<double> &arr) {
  const std::size_t n = arr.size();
  for (std::size_t i = 0; i < n - 1; ++i)
    for (std::size_t j = 0; j < n - i - 1; ++j)
      if (arr[j] > arr[j + 1])
        std::swap(arr[j], arr[j + 1]);
}

//question 4-4
void hellohello() {
  // STL List Operations
  //  Initialize lists
  std::list<int> L1, L2;
  for (int i = 0; i < 10; ++i) // fill L1 with 0 to 9
    L1.push_back(i);
  for (int i = 10; i < 20; ++i) // fill L2 with 10 to 19
    L2.push_back(i);
  L1.splice(L1.end(), L2); // Moves elements from L2 to L1
  cout << "Print sizes of L1: " << L1.size() << " and L2 : " << L2.size()
       << endl;
  L1.remove_if([](int x) {
    return x % 2 == 0;
  }); // to remove even numbers efficiently from L1 with lambda function
  if (!L1.empty()) { // check if list is not empty
    cout << "First element of L1: " << L1.front() << endl;
    cout << "Last element of L1: " << L1.back() << endl;
  }
}

//question 4-5
void find_primes() {
  // given
  std::vector<bool> sieve(1000000, true); // true = prime
  std::vector<int> primes;
  primes.reserve(80000); // to pre-allocate space for about 80000 primes

  // sieve of eratosthenes
  sieve[0] = sieve[1] = false; // base case: 0 and 1 are not prime numbers
  for (int num = 2; num * num < 1000000;
       ++num) { // check after 2 up to sqrt(1000000)
    if (sieve[num]) {
      for (int multiple = num * num; multiple < 1000000; multiple += num) {
        sieve[multiple] = false; // mark not prime = composite
      }
    }
  }
  for (int i = 2; i < 1000000; ++i) {
    if (sieve[i]) {// if prime
      primes.push_back(i); // collecting prime numbers
    }
  }
  std::cout << "Number of primes found less than 10000000: " << primes.size()
            << std::endl;
}

//question 4-6
void storeaccessmatrix(){
    // given 
    int rows = 3;
    int cols = 4;

    //using method 1: 2d vector 
    std::vector<std::vector<int>> matrix_2d = {
        {34,  57,  12,  99},
        {50,  16,  11,  98},
        {90, 15, 10, 97} // just some random number
    };

    for (const auto& row: matrix_2d){ //row
        for (const auto& element: row){
            cout << element << " ";
        }
        cout << "\n";
    }
    cout << endl;

    for (int j = 0; j < cols; ++j){ //column
        for (int i = 0; i < rows; ++i){
            cout << matrix_2d[i][j] << " ";
        }
        cout << "\n";
    }
    cout << endl;

    //using method 2: 1d vector 
    std::vector<int> matrix_1d(rows * cols);
    int value1 = 1;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix_1d[i * cols + j] = value1++; // try to fill with 1 2 ... 12
        }
    }

    for (int i = 0; i < rows; ++i) { //row
        for (int j = 0; j < cols; ++j) {
            cout << matrix_1d[i * cols + j] << " ";
        }
        cout << "\n";
    }
    cout << endl;

    for (int j = 0; j < cols; ++j) { //column
        for (int i = 0; i < rows; ++i) {
            cout << matrix_1d[i * cols + j] << " ";
        }
        cout << "\n";
    }
    cout << endl;

}
//question 4-7
void replace_list(list<int>& L) {
    // Method 2 from the clue: STL algorithm
    replace(L.begin(), L.end(), 0, 1);

    /* // Method 1 (manual) — equivalent and also OK:
    for (int& x : L) {
        if (x == 0) x = 1;
    }
    */
}

// --- demo / test ---
void print_list(const list<int>& L) {
    bool first = true; // to avoid leading '-'
    for (int x : L) {
        if (!first) cout << '-';
        cout << x;
        first = false;
    }
    cout << '\n';
}

void q4_7() {
    // Case 1: Mixed values
    list<int> L1 = {0, 0, 1, 3, 0, 5, 0, 6};
    cout << "Case 1: Mixed values\nInput:  ";
    print_list(L1);
    replace_list(L1);
    cout << "Output: "; print_list(L1);
    cout << "----------------------\n";

    // Case 2: All zeros
    list<int> L2 = {0, 0, 0, 0};
    cout << "Case 2: All zeros\nInput:  ";
    print_list(L2);
    replace_list(L2);
    cout << "Output: "; print_list(L2);
    cout << "----------------------\n";

    // Case 3: No zeros
    list<int> L3 = {2, 4, 6, 8};
    cout << "Case 3: No zeros\nInput:  ";
    print_list(L3);
    replace_list(L3);
    cout << "Output: "; print_list(L3);
    cout << "----------------------\n";

    // Case 4: Empty list
    list<int> L4;
    cout << "Case 4: Empty list\nInput:  ";
    print_list(L4);
    replace_list(L4);
    cout << "Output: "; print_list(L4);
    cout << "----------------------\n";
}

//question 4-8
void print_vector(const vector<int>& vec) {
    cout << "Vector contents: ";
    for (const auto& element : vec) { // range-based for loop
        cout << element << " ";
    }
    cout << "(size: " << vec.size() << ")\n";
}

void q4_8() {
    // 1. Create vector of length 10 (default initialized to 0)
    vector<int> vec(10);
    cout << "Step 1: Initial vector of length 10\n";
    print_vector(vec); // should show 10 zeros

    // 2. Append two new values
    vec.push_back(100); // append 100
    vec.push_back(200); // append 200
    cout << "Step 2: After appending two values (100, 200)\n";
    print_vector(vec); // should show 10 zeros followed by 100 and 200

    // 3. Remove last element
    vec.pop_back(); // remove last element (200)
    cout << "Step 3: After removing last element\n";
    print_vector(vec); // should show 10 zeros followed by 100

    // 4. Change size to 20
    vec.resize(20); // resize to 20 (new elements default to 0)
    cout << "Step 4: After resizing to 20\n";
    print_vector(vec); // should show 10 zeros, 100, and then 9 more zeros

    // 5. Remove all entries
    vec.clear(); // clear all elements
    cout << "Step 5: After clearing all elements\n";
    print_vector(vec); // should show empty vector
}
  // Create an integer array perm of length 100 with entries 0,1,...,99 in this order. 
  vector<int> generate_permutation() {
    vector<int> perm(100);
    iota(perm.begin(), perm.end(), 0); // fill with 0,1,...,99

    random_device rd; // obtain a random number from hardware
    mt19937 gen(rd()); // seed the generator

    for (int i = 99; i > 0; --i) { // for each i = 99, 98,..,1
      uniform_int_distribution<> dis(0, i); // define the range
      int j = dis(gen); // determine a random index j in the range 0,...,i
      swap(perm[i], perm[j]); // use std swap the entries perm[i] and perm[j]
    }
    return perm;
  }
  // for each i = 99, 98,..,1 , determine a random index j in the range 0,...1:, use std swap the entries perm[i] and per[j]

  //write a function that generates a random permutation of 0,1,..,99
  vector<int> generate_permutation() {
    vector<int> perm(100);
    iota(perm.begin(), perm.end(), 0); // fill with 0,1,...,99

    random_device rd; // obtain a random number from hardware
    mt19937 gen(rd()); // seed the generator

    for (int i = 99; i > 0; --i) { // for each i = 99, 98,..,1
      uniform_int_distribution<> dis(0, i); // define the range
      int j = dis(gen); // determine a random index j in the range 0,...,i
      swap(perm[i], perm[j]); // use std swap the entries perm[i] and perm[j]
    }
    return perm;
  }

  //write a test function that generate 1000 permutation
  void test_permutation() {
    for (int i = 0; i < 1000; ++i) {
      auto perm = generate_permutation();
      // Print or validate the permutation
      for (const auto& val : perm) {
        cout << val << ' ';
      }
      cout << '\n'; // Print a newline after each permutation
    }
  }

//question 4-9
using Grid = vector<vector<int>>; //Grid is a 2D vector of integers
// using the similar way with q4_6
void printrows(const Grid &g) {
  for (int r = 0; r < 9; ++r) { // for each row
    cout << "Row " << r + 1 << ":"; // print row number
    for (int c = 0; c < 9; ++c) // for each column
      cout << g[r][c] << " "; // print the element
    cout << "\n"; 
  }
}

void printcols(const Grid &g) { 
  for (int c = 0; c < 9; ++c) { // for each column
    cout << "Col " << c + 1 << ":"; // print column number
    for (int r = 0; r < 9; ++r) // for each row
      cout << g[r][c] << " "; // print the element
    cout << "\n";
  }
}

void printboxes(const Grid &g) {
  for (int br = 0; br < 3; ++br) // for each box row
    for (int bc = 0; bc < 3; ++bc) { // for each box column
      cout << "Box (" << br + 1 << "," << bc + 1 << "): "; // print box coordinates
      for (int r = 0; r < 3; ++r) // for each row in the box
        for (int c = 0; c < 3; ++c)   // for each column in the box
          cout << g[3 * br + r][3 * bc + c] << ' '; // to print the 3 x 3 boxes
      cout << '\n';
    }
}

bool sum45(const Grid &g) {
  const int SUM = 45; // sum of numbers 1 to 9

  // check sum of rows
  for (int r = 0; r < 9; ++r) {
    int s = 0;  // sum for the current row
    for (int c = 0; c < 9; ++c)   // for each column in the row
      s += g[r][c];   // accumulate the sum
    if (s != SUM)
      return false;
  }

  // check sum of columns
  for (int c = 0; c < 9; ++c) {
    int s = 0;  // sum for the current column
    for (int r = 0; r < 9; ++r)
      s += g[r][c];   // accumulate the sum
    if (s != SUM) 
      return false;
  }

  // check sum of boxes
  for (int br = 0; br < 3; ++br) // for each box row
    for (int bc = 0; bc < 3; ++bc) { // for each box column
      int s = 0;  // sum for the current box
      for (int r = 0; r < 3; ++r) // for each row in the box
        for (int c = 0; c < 3; ++c)
          s += g[3 * br + r][3 * bc + c];
      if (s != SUM)
        return false;
    }

  return true;
}

//question 4-10
struct RateCurve {
    std::vector<int> dates; // in serial date number
    std::vector<double> rates; // in percentage
};

// Interpolation of zero rate curve
double interpolate(const std::vector<int> &dates,
                   const std::vector<double> &rates, int end_date) {
  for (std::size_t i = 0; i + 1 < dates.size(); ++i) {
    if (end_date >= dates[i] && end_date <= dates[i + 1]) {
      double t = double(end_date - dates[i]) / double(dates[i + 1] - dates[i]);
      return rates[i] + t * (rates[i + 1] - rates[i]);
    }
  }
  return rates.back();
}

// Discount factor
double get_df(int end_date, const RateCurve &ic, int today) {
  double r = interpolate(ic.dates, ic.rates, end_date);
  double days = double(end_date - today);
  return std::exp(-days / 365.0 * r / 100.0);
}

// Bond pricing with zero curve
double bond_price_zero(double face_value, double coupon_rate, double maturity,
                       const std::string &ps, int today, const RateCurve &ic,
                       double /*ic_spread*/) {
  int k = 1;
  if (ps == "S")
    k = 2;
  else if (ps == "Q")
    k = 4;

  double cpn = face_value * coupon_rate / k;
  double price = 0.0;

  int total = int(maturity * k);
  for (int j = 1; j <= total; ++j) {
    int end_date = today + (365 / k) * j;
    price += cpn * get_df(end_date, ic, today);
  }
  int mat_date = today + int(365 * maturity);
  price += face_value * get_df(mat_date, ic, today);
  return price;
}

// Define IC here
const RateCurve IC{{41082, 41083, 41085, 41092, 41116, 41146, 41179,
                    41188, 41278, 41368, 41461, 41552, 41649, 41816,
                    42181, 42546, 42911, 43279, 43643, 44007, 44372,
                    44738, 45470, 46564, 48390, 50216, 52043},
                   {0.0,
                    0.344720594377147,
                    0.344720594377147,
                    0.368368776205730,
                    0.525570770718078,
                    0.5898583267423878,
                    0.741119832110955,
                    0.69674990102990,
                    0.80665936532139,
                    0.884475547507158,
                    0.941966915221572,
                    0.99201621992792,
                    1.04351212131796,
                    1.34312007757781,
                    1.59973094475305,
                    1.86320995459478,
                    2.11609386013679,
                    2.34400261226439,
                    2.53599371340597,
                    2.70067752825491,
                    2.83822638195572,
                    2.95830187616262,
                    3.16075166306880,
                    3.33878283257801,
                    3.40297375743186,
                    3.29434168467002,
                    3.11704336212427}};



     


int main(){
    // q4_1
  cout << "\nThis is for question 4-1\n";
  cout << "Test 1 gcd(48,18)= " << gcd(48, 18) << endl;
  cout << "Test 2 gcd(0,5)= " << gcd(0, 5) << ", and gcd(7,0)= " << gcd(7, 0)
       << endl;
  cout << "Test 3 gcd(1071,462) = " << gcd(1071, 462) << endl;

  // q4_2
  cout << "\nThis is for question 4-2\n";
  cout << "This is the normal version\n";
  cout << "p(5) = " << p(5) << endl;
  cout << "p(50) = " << p(50) << endl;

  cout << "This is the memoization version\n";
  cout << "p(4)  = " << p_memo(4) << "\n";
  cout << "p(5)  = " << p_memo(5) << "\n";
  cout << "p(50) = " << p_memo(50) << "\n";
    cout << "\n Hello, This is for Assignment 4" << '\n';
    // q4_3
  cout << " This is for question 4-3\n";
  vector<double> arr = {33.0, 23.0, 1.0, 5.0, 3.0, 2.0};
  bubble_sort(arr);
  for (double x : arr)
    cout << x << ' ';
  cout << "\n\n";

  // q4_4
  cout << " This is for question 4-4\n";
  hellohello();
  cout << "\n";

  // q4_5
  cout << " This is for question 4-5\n";
  find_primes();
  cout << "\n";

  // q4_6
  cout << " This is for question 4-6\n";
  storeaccessmatrix();
  cout << "\n";
  
  // q4_7
  cout << " This is for question 4-7\n";
  q4_7();
  cout << "\n";

  // q4_8
  cout << " This is for question 4-8\n";
  q4_8();
  cout << "\n";

  // q4_9
  cout << " This is for question 4-9\n";
  Grid sudoku = {{8, 9, 5, 7, 4, 1, 6, 2, 3}, {2, 4, 3, 9, 5, 6, 8, 1, 7},
                 {7, 6, 1, 8, 2, 3, 4, 9, 5}, {9, 3, 4, 6, 7, 5, 1, 8, 2},
                 {6, 1, 7, 2, 9, 8, 5, 3, 4}, {5, 8, 2, 3, 1, 4, 9, 7, 6},
                 {3, 5, 9, 1, 6, 2, 7, 4, 8}, {1, 2, 6, 4, 8, 7, 3, 5, 9},
                 {4, 7, 8, 5, 3, 9, 2, 6, 1}};
  printrows(sudoku);
  printcols(sudoku);
  printboxes(sudoku);
  cout << "\nChecking Sudoku\n" << (sum45(sudoku) ? "PASS" : "FAIL") << "\n\n";

  // q4_10
  cout << " This is for question 4-10\n";

  std::cout << std::fixed << std::setprecision(3);
  auto v00 = bond_price_zero(100, 0.02, 4, "Q", 41082, IC, 0.0);
  cout << "v00: " << v00 << "   // expected ~100.579\n";

  auto v01 = bond_price_zero(100, 0.03, 2, "S", 41082, IC, 0.0);
  cout << "v01: " << v01 << "   // expected ~103.281\n";

  auto v02 = bond_price_zero(100, 0.04, 3, "A", 41082, IC, 0.0);
  cout << "v02: " << v02 << "   // expected ~106.992\n";

}