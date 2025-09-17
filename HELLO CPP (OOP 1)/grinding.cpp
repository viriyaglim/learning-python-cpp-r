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

//assignment 1

// Question 1-1
void q1_1() {
  std::cout << "\n"
               "\n"
            << "This is for Question 1-1" << std::endl;
  // Requirements
  // Use a variable to store the number and std::string to store the rest of the
  // message
  auto number = 1111111111111111111;
  std::string message = " is a prime number!";
  // If you use auto for the number variable, what type is it? You may need to
  // define a few variables to find out what their typeid are
  std::cout << number << message << std::endl;
  // Use typeid(variable).name() to determine the actual type
  std::cout << "Type of number is  " << typeid(number).name() << std::endl;
  std::cout << "Type of message is " << typeid(message).name() << std::endl;
}

// Question 1-2
void q1_2() {
  std::cout << "\n"
               "\n"
            << "This is for Question 1-2" << std::endl;
  /*
  int i2 = 1024 * 1024 * 1024;
  cout << i2 * 2 << '\n';
  double d1 = 1024 * 1024 * 1024 * 4;
  cout << d1 << '\n';
  cout << d1 * 2 << '\n';
  auto u1 = 25u - 50;
  cout << u1 << '\n';
  */
  // Any idea what is happening? Write your answer in the comments.

  std::cout << "My explanation answer is in the comments of the code.:D";
  // i2 explanation
  // i2 results into (2^10)^3 = 2^30
  // From slide 106/146, i2 * 2 which is 2147483648, which is also 2^31, this
  // exceeds the range of int [-2^31, 2^31-1] and results in an overflow.
  // because it exceeds the range of int by 1, it reaches the other end of the
  // range and it is considered as the starting number from the negative range
  // of int, which is -2^31. the value wraps around due to overflow RESULT:
  // -2147483648 (-2^31)

  // d1 explanation
  // d1 calculation is done in int, resulting into (2^10)^3 * 4 = 2^32, before
  // assigned to double type. From slide 106/146, 2^32 exceeds the range of int
  // [-2^31, 2^31-1], resulting in integer overflow before it was assigned to
  // double type, 2^32 exceeds the range of int by 2^32 - (2^31-1) = 2^31 + 1,
  // it reaches the other end of the range and it wraps around to 0 resulting in
  // 0 and it is assigned to double. RESULT: 0

  // d2 explanation
  // d2 calculation is done in int, resulting into (2^10)^3 * 4 * 2 = 2^33,
  // before assigned to double type. From slide 106/146, 2^33 exceeds the range
  // of int [-2^31, 2^31-1], resulting in integer overflow before it was
  // assigned to double type. 2^33 exceeds the range of int by 2^33 - (2^31-1) =
  // 3*2^31 + 1, it reaches the other end of the range and it wraps around to 0.
  // resulting in 0 and it is assigned to double.
  // RESULT: 0

  // u1 explanation
  // u1 results into 25u - 50 = -25
  // 25u is unsigned int, 50 is signed int
  // From slide 108/146 (When types interact), we have the info that when int +
  // unsigned int => unsigned int resulting in u1 becomes unsigned int as the
  // subtraction is done in unsigned as the wider range type RESULT: 4294962721
  // (unsigned underflow)
}

void q3_1() {
  std::cout << "\n"
               "\n"
            << "This is for Question 1-3" << std::endl;
  // Given from slide 136-146/146
  ////1. Include VoucherV3, HouseHold, and HouseholdVouchersV2 declared in the
  /// lecture notes slides. Make them work together.
  enum class VoucherValue { ONE = 1, TWO = 2, FIVE = 5, TEN = 10 };
  struct VoucherV3 {
    VoucherValue value;
    bool spent = false;
  };

  struct Household {
    long id;
    std::string address;
  };

  using HouseholdID = long;

  struct HouseholdVouchersV2 {
    HouseholdID id;
    vector<VoucherV3> vouchers;
    int balance = 0;
    bool issued = false;
    int issued_amount = 0;
  };

  // new code
  // vouchers to issue
  vector<VoucherV3> voucherpack = {
      {VoucherValue::TWO},  {VoucherValue::TWO},  {VoucherValue::TWO},
      {VoucherValue::TWO},  {VoucherValue::TWO},  {VoucherValue::FIVE},
      {VoucherValue::FIVE}, {VoucherValue::FIVE}, {VoucherValue::FIVE},
      {VoucherValue::TEN},  {VoucherValue::TEN}};

  // 2. Create three households, h1, h2, h3, each with a different address and
  // ID. Use your own data. example from slide: Household h{1234, "Nanyang
  // Hill"};
  Household h1{1001, "Pioneer"};
  Household h2{1002, "Boon Lay"};
  Household h3{1003, "Chinese Garden"};

  HouseholdVouchersV2 hv1{h1.id}, hv2{h2.id}, hv3{h3.id};// HouseholdVouchersV2
                                                        // for each household

  // 3. Issue each household $50 worth of vouchers: $2 (5 vouchers), $5 (4
  // vouchers), $10 (2 vouchers)
  hv1.vouchers = voucherpack;
  hv2.vouchers = voucherpack;
  hv3.vouchers = voucherpack;
  hv1.issued = hv2.issued = hv3.issued = true;
  hv1.balance = hv2.balance = hv3.balance = 50;
  hv1.issued_amount = hv2.issued_amount = hv3.issued_amount = 50;

  cout << "Initial balances for h1="
       << "$" << hv1.balance << ", h2="
       << "$" << hv2.balance << ", h3="
       << "$" << hv3.balance << '\n';

  // 4. Update h1 to simulate spending $20 using any combination of vouchers.
  // Update all relevant fields in the variable.

  // if the voucher $10*2 are spent
  hv1.vouchers[9].spent = true;
  hv1.vouchers[10].spent = true;

  // Recalculate balance for h1 by counting only not spent vouchers
  int newbalance = 0;
  for (auto &v : hv1.vouchers) {
    if (!v.spent) {
      newbalance += static_cast<int>(v.value);// sum remaining vouchers
    }
  }
  hv1.balance = newbalance;

  std::cout << "h1 spent $20 using two $10 vouchers. Remaining balance = $"
            << hv1.balance << '\n';

  // Confirm the remaining (unused) vouchers of each household.
  auto remainingvouchers = [](const std::string &name,
                              const HouseholdVouchersV2 &hv) {
    int total = 0;
    std::cout << name << " remaining vouchers: ";
    for (const auto &v : hv.vouchers) { // for each voucher
      if (!v.spent) { // if not spent
        std::cout << "$" << static_cast<int>(v.value)
                  << " ";                   // print every voucher remaining
        total += static_cast<int>(v.value); // sum remaining vouchers
      }
    }
    std::cout << " | Total balance: $" << total << "\n";
  };
  remainingvouchers("h1", hv1);
  remainingvouchers("h2", hv2);
  remainingvouchers("h3", hv3);
}

// Compound interest (Formula)
double compoundAmount(double P, double r, int t) { return P * pow(1 + r, t); }

void q4_1() {
  std::cout << "\n"
               "\n"
            << "This is for Question 1-4" << std::endl;
  // Struct Loan and Deposit
  struct Loan {
    const double principal;
    int term_years;
    const double annual_rate;
    double interest_due;
  };

  struct Deposit {
    const double principal;
    int term_years;
    const double annual_rate;
    double interest_earned;
  };

  // 1) Create two variables of the structs representing the current state.
  Loan loan_now{300000, 20, 0.031, 0.0};
  Deposit deposit_now{100000, 5, 0.025, 0.0};

  // Calculate initial interest (t=0 means no interest yet)
  loan_now.interest_due =
      compoundAmount(loan_now.principal, loan_now.annual_rate, 0) -
      loan_now.principal;// calculate interest due
  deposit_now.interest_earned =
      compoundAmount(deposit_now.principal, deposit_now.annual_rate, 0) -
      deposit_now.principal;

  // 2. Move forward to 4 years later. Create two more variables as copies of
  // the previous variables. Update their relevant fields to reflect the
  // situation after 4 years.
  Loan loan_after4 = loan_now;
  Deposit deposit_after4 = deposit_now;

  loan_after4.term_years -= 4;    // reduce remaining term
  deposit_after4.term_years -= 4; // reduce remaining term

  // 3. Print the values from both the original and updated structs.
  double loan_total_after4 =
      compoundAmount(loan_after4.principal, loan_after4.annual_rate, 4);
  double deposit_total_after4 =
      compoundAmount(deposit_after4.principal, deposit_after4.annual_rate, 4);

  loan_after4.interest_due = loan_total_after4 - loan_after4.principal;
  deposit_after4.interest_earned =
      deposit_total_after4 - deposit_after4.principal;

  // 3. Print the values from both the original and updated structs and
  // 4. Calculate and print the interest due/interest earned. Interest is
  // calculated annually with compounding.
  cout << fixed;
  cout << "---- Now ----\n";
  cout << "Loan: Principal = $" << loan_now.principal
       << ", Term = " << loan_now.term_years
       << " years,"
          "\n"
       << "Interest Rate = " << loan_now.annual_rate * 100 << "%"
       << ", Interest Due = $" << loan_now.interest_due << "\n";
  cout << "Deposit: Principal = $" << deposit_now.principal
       << ", Term = " << deposit_now.term_years
       << " years,"
          "\n"
       << "Interest Rate = " << deposit_now.annual_rate * 100 << "%"
       << ", Interest Earned = $" << deposit_now.interest_earned << "\n\n";

  cout << "---- After 4 Years ----\n";
  cout << "Loan: Principal = $" << loan_after4.principal
       << ", Remaining Term = " << loan_after4.term_years
       << " years,"
          "\n"
       << "Interest Rate = " << loan_after4.annual_rate * 100 << "%"
       << ", Interest Due = $" << loan_after4.interest_due << "\n";
  cout << "Deposit: Principal = $" << deposit_after4.principal
       << ", Remaining Term = " << deposit_after4.term_years
       << " years"
          "\n"
       << "Interest Rate = " << deposit_after4.annual_rate * 100 << "%"
       << ", Interest Earned = $" << deposit_after4.interest_earned << "\n";
}

// assignment 2 

void q2_1() {
  cout << "This is for question 2-1." << endl;

  // Function to calculate weekday using Zeller's algorithm
  auto calculate_weekday = [](const int day, const int month, const int year) {
    int q = day;
    int m = month;
    int y = year;
    // Starting from March, need to adjust for January and February
    if (m == 1) { // January
      m = 13;
      y--;
    } else if (m == 2) { // February
      m = 14;
      y--;
    }

    int K = y % 100;
    int J = y / 100;

    // Zeller's formula from Wikipedia
    int w = (q + static_cast<int>(floor((13 * (m + 1)) / 5.0)) + K +
             static_cast<int>(floor(K / 4.0)) +
             static_cast<int>(floor(J / 4.0)) + (5 * J)) %
            7;

    // From algorithm details, w = 0 to be Saturday, 1 to be Sunday, and so on
    // until 6 to be Friday
    return w;
  };

  // Write a test function for a few specific dates (given from problem
  // statement)
  auto test_zeller_spot = [&]() {
    cout << boolalpha;
    // Was 1 Jan 2000 a Saturday? True
    cout << (calculate_weekday(1, 1, 2000) == 0) << '\n';
    // Was 26 Jan 2004 (my birthdate) was a Monday? True
    cout << (calculate_weekday(26, 1, 2004) == 2) << '\n';
    // Will 25 Dec 2025 (this Christmas) be a Friday? False
    cout << (calculate_weekday(25, 12, 2025) == 6) << '\n';
    // Was 17 Aug 1945 (Indonesia Independence Day) a Monday? False
    cout << (calculate_weekday(17, 8, 1945) == 2) << '\n';
    // Was 29 Feb 2020 a Monday? False
    cout << (calculate_weekday(29, 2, 2020) == 2) << '\n';
  };

  // Write another test function to test the date range 2023-08-14 to
  // 2024-08-06.
  auto test_date_range = [&]() {
    try {
      for (int y = 2023; y <= 2024; y++) {
        for (int m = 1; m <= 12; m++) {
          for (int d = 1; d <= 31; d++) {
            if (y == 2023 && (m < 8 || (m == 8 && d < 14)))
              continue; // to skip dates before 2023-08-14
            if (y == 2024 && (m > 8 || (m == 8 && d > 6)))
              continue; // to skip dates after 2024-08-06

            int w = calculate_weekday(d, m, y); // calculate weekday
            assert(w >= 0 && w <= 6); // w should be in range 0 to 6
          } 
        }
      }
      cout << "Done the date range test!\n";
    } catch (...) { /*
      GenAI Assistance:
      - Recommended by ChatGPT to use try–catch for error handling */
      cout << "Error in date range test.\n";
    }
  };

  // Run both tests
  test_zeller_spot();
  test_date_range();
}

void q2_2() {
  cout << endl << "This is for question 2-2." << endl;
  int n{1000};
  while (n >= 0) {
    n -= 13;
  }
  cout << "The final value of the integer is " << n << "."
       << endl; // Final result will be -1 because 1000 - 13*77 = -1.
}

void q2_3() {
  cout << endl << "This is for question 2-3." << endl;
  cout << "Part 1. Print all pairs (a,b) where a = 1, …,10 and b = 1,…,10 such "
          "that a ≠ b."
       << endl;
  for (int a = 1; a <= 10; a++) {
    for (int b = 1; b <= 10; b++) {
      if (a != b) {
        cout << "(" << a << ", " << b << ")" << endl;
      }
    }
  }

  cout << endl
       << "Part 2. Write a program that prints the numbers from 1 to 100, "
          "except numbers which are multiples of 7 or 11."
       << endl;
  for (int x = 1; x <= 100; x++) {
    if (x % 7 != 0 && x % 11 != 0) {
      cout << x << ", ";
    }
  }
}

void q2_4() {
  cout << endl << endl << "This is for question 2-4." << endl;
  /* GenAI Assistance: - Recommend me to use setprecision to format the output
   * to max 2 decimal places. */
  // Problem parameters
  const double A = 100000.0;  // loan amount
  const int N = 12;           // number of months
  const double ia = 0.05;     // annual interest rate
  const double i = ia / 12.0; // monthly rate

  // Format from the expected output
  auto print_line = [](int curr_loan_month, double curr_payment,
                       double curr_interest_payment,
                       double curr_principal_payment, double curr_balance) {
    cout << "CM: " << curr_loan_month << " CPI: " << fixed << setprecision(2)
         << curr_payment << " CI: " << curr_interest_payment
         << " CP: " << curr_principal_payment << " Bal: " << curr_balance
         << '\n';
  };

  // 1) Bullet / Balloon payment
  auto bullet = [&]() {
    cout << "Bullet Payment:\n";

    double curr_balance = A;
    double all_payments = 0.0, all_interest = 0.0, all_principal = 0.0;
    // totals

    // Print initial amount
    print_line(0, 0.0, 0.0, 0.0, curr_balance);

    for (int curr_loan_month = 1; curr_loan_month <= N; ++curr_loan_month) {
      double curr_interest_payment = curr_balance * i;
      double curr_principal_payment =
          (curr_loan_month == N ? curr_balance : 0.0);
      double curr_payment = curr_interest_payment + curr_principal_payment;
      curr_balance -= curr_principal_payment;

      all_payments += curr_payment;
      all_interest += curr_interest_payment;
      all_principal += curr_principal_payment;

      print_line(curr_loan_month, curr_payment, curr_interest_payment,
                 curr_principal_payment, curr_balance);
    }

    cout << "Total PI: " << fixed << setprecision(2) << all_payments
         << " I: " << all_interest << " P: " << all_principal << "\n\n";
  };

  // 2) Equal principal payment
  auto equal_principal = [&]() {
    cout << "Equal Principal Payment:\n";

    double curr_balance = A;
    double all_payments = 0.0, all_interest = 0.0, all_principal = 0.0;
    const double curr_principal_payment = A / N;

    // Print initial amount
    print_line(0, 0.0, 0.0, 0.0, curr_balance);

    for (int curr_loan_month = 1; curr_loan_month <= N; ++curr_loan_month) {
      double curr_interest_payment = curr_balance * i;
      double curr_payment = curr_principal_payment + curr_interest_payment;
      curr_balance -= curr_principal_payment;

      all_payments += curr_payment;
      all_interest += curr_interest_payment;
      all_principal += curr_principal_payment;

      print_line(curr_loan_month, curr_payment, curr_interest_payment,
                 curr_principal_payment, curr_balance);
    }

    cout << "Total PI: " << fixed << setprecision(2) << all_payments
         << " I: " << all_interest << " P: " << all_principal << "\n\n";
  };

  // 3) Equal total payment
  auto equal_total = [&]() {
    cout << "Equal Total Payment:\n";

    const double PMT = A * (i / (1.0 - pow(1.0 + i, -N)));

    double curr_balance = A;
    double all_payments = 0.0, all_interest = 0.0, all_principal = 0.0;

    // Print initial amount
    print_line(0, 0.0, 0.0, 0.0, curr_balance);

    for (int curr_loan_month = 1; curr_loan_month <= N; ++curr_loan_month) {
      double curr_interest_payment = curr_balance * i;
      double curr_principal_payment = PMT - curr_interest_payment;
      double curr_payment = PMT;
      curr_balance -= curr_principal_payment;

      all_payments += curr_payment;
      all_interest += curr_interest_payment;
      all_principal += curr_principal_payment;

      if (curr_loan_month == N && curr_balance < 1e-8)
        curr_balance = 0.0;

      print_line(curr_loan_month, curr_payment, curr_interest_payment,
                 curr_principal_payment, curr_balance);
    }

    cout << "Total PI: " << fixed << setprecision(2) << all_payments
         << " I: " << all_interest << " P: " << all_principal << "\n\n";
  };

  // Output results for each payment type
  bullet();
  equal_principal();
  equal_total();
}
////q3_1 
//Write a program to print a table of values for the function 
double f(double x) {
    return pow(sin(x),2) + exp(-pow(x,2)) + pow(x,3) * log(x+1);
}

//q3_2
// Part 1: multiply two integers safely using long long
long long multiply_integers(int a, int b) {
    return static_cast<long long>(a) * b; // cast to long long to prevent overflow
}

// Part 2: check if two integers have the same parity (both even or both odd)
bool same_parity(int a, int b) {
    return (a % 2 == b % 2);
}

// --- demo / test ---
void q3_2() {
    // Testing multiply_integers
    cout << "Multiply 1000 * 2000 = " << multiply_integers(1000, 2000) << "\n";
    cout << "Multiply 123456 * 987654 = " << multiply_integers(123456, 987654) << "\n";

    // Testing same_parity
    cout << "Do 4 and 8 have same parity? " << (same_parity(4, 8) ? "true" : "false") << "\n"; // true
    cout << "Do 3 and 7 have same parity? " << (same_parity(3, 7) ? "true" : "false") << "\n"; // true
    cout << "Do 2 and 5 have same parity? " << (same_parity(2, 5) ? "true" : "false") << "\n"; // false
}

//q3_3
// Write and test a function:
vector<double> rand_vector(int n, double a=0.0, double b = 1.0){
    vector<double> v(n); //vector of size n with default values 0.0
    random_device rd; // obtain a random number from hardware
    mt19937 gen(rd());
    uniform_real_distribution<double> uni(a,b);
    for (int i=0; i<n; i++){
        v[i] = uni(gen); //generate random number
    }
    return v;
}

 
//q3_4
//Part 1: Write a function void change_x(int x) that attempts to increase the value of x by 1.
void change_x(int x){ x = x + 1; } //example: int a=5; change_x(a); // a is still 5, what changes is a copy of a passed to the function

// Part 2: Write a function void change_x_with_ref(int& x) that increases the value of x by 1.
void change_x_with_ref(int& x){ x = x + 1;} //example: int a=5; change_x_with_ref(a); // a is now 6, because a reference to a is passed to the function

// Part 3: Write a function void value_copy(std::vector<int> v) that does nothing and call it 1000 times in the main function with a vector of size 1 million as input. 
void value_copy(std::vector<int> v){} // Measure and note the program execution time.

// Part 4: Create a new function void value_copy_with_ref(const std::vector<int>& v) to do the same task as value_copy, and measure the running time.
void value_copy_with_ref(const std::vector<int>& v){}

//q3_5
// Function: adds two vectors element-wise
tuple<bool, vector<double>> add_vectors(
    const vector<double>& vec1,
    const vector<double>& vec2)
{
    if (vec1.size() != vec2.size()) {
        cerr << "Error: Vectors must have the same size!\n";
        return {false, {}}; // return false + empty vector
    }

    vector<double> result(vec1.size()); // empty vector of same size
    for (size_t i = 0; i < vec1.size(); ++i) {
        result[i] = vec1[i] + vec2[i];
    }

    return {true, result};
}

void q3_5() {
    vector<double> v1 = {1.0, 2.0, 3.0};
    vector<double> v2 = {4.0, 5.0, 6.0};
    vector<double> v3 = {7.0, 8.0}; // wrong size

    auto [ok1, res1] = add_vectors(v1, v2);
    if (ok1) {
        cout << "v1 + v2 = ";
        for (double x : res1) cout << x << " ";
        cout << "\n";
    }

    auto [ok2, res2] = add_vectors(v1, v3);
    if (!ok2) {
        cout << "Failed to add v1 + v3 due to size mismatch\n";
    }
}

//q3_6
/*
// === Helpers ===
inline int to_int(VoucherValue v) { return static_cast<int>(v); }

inline int compute_unspent_balance(const HouseholdVouchersV2& hv) {
    int s = 0;
    for (const auto& vc : hv.vouchers) if (!vc.spent) s += to_int(vc.value);
    return s;
}

// Build a voucher pack by counts (e.g., 5×$2, 4×$5, 2×$10)
inline vector<VoucherV3> make_pack(int c1, int c2, int c5, int c10) {
    vector<VoucherV3> p;
    p.reserve(c1 + c2 + c5 + c10);
    while (c1--  > 0) p.push_back({VoucherValue::ONE});
    while (c2--  > 0) p.push_back({VoucherValue::TWO});
    while (c5--  > 0) p.push_back({VoucherValue::FIVE});
    while (c10-- > 0) p.push_back({VoucherValue::TEN});
    return p;
}

// ====================== ADMIN APIs ======================

// Issue once (simple policy): refuse if already issued
inline bool issue_voucher_pack(HouseholdVouchersV2& hv, const vector<VoucherV3>& pack) {
    if (pack.empty()) return false;
    if (hv.issued) { std::cerr << "Already issued to household " << hv.id << "\n"; return false; }
    hv.vouchers.insert(hv.vouchers.end(), pack.begin(), pack.end());
    hv.issued = true;
    for (auto& v : pack) hv.issued_amount += to_int(v.value);
    hv.balance = compute_unspent_balance(hv);
    return true;
}

// Top-up (allow multiple times)
inline bool topup_voucher_pack(HouseholdVouchersV2& hv, const vector<VoucherV3>& pack) {
    if (pack.empty()) return false;
    hv.vouchers.insert(hv.vouchers.end(), pack.begin(), pack.end());
    for (auto& v : pack) hv.issued_amount += to_int(v.value);
    hv.balance = compute_unspent_balance(hv);
    return true;
}

// ===================== MERCHANT APIs =====================

struct RedemptionResult {
    bool ok{};
    int redeemed_amount{};
    vector<int> used_indices;     // indices in hv.vouchers marked spent
    string message;
};

// Greedy exact-change picker over available unspent vouchers.
// Works well with (10,5,2,1); fails if exact sum not possible with what’s left.
inline RedemptionResult pick_exact(const HouseholdVouchersV2& hv, int amount) {
    if (amount <= 0) return {false, 0, {}, "Amount must be positive"};

    // Collect unspent vouchers {value, index} and sort high->low
    vector<pair<int,int>> pool;
    pool.reserve(hv.vouchers.size());
    for (int i = 0; i < (int)hv.vouchers.size(); ++i)
        if (!hv.vouchers[i].spent) pool.emplace_back(to_int(hv.vouchers[i].value), i);
    sort(pool.begin(), pool.end(), [](auto& a, auto& b){ return a.first > b.first; });

    vector<int> used;
    int left = amount;
    for (auto [val, idx] : pool) {
        while (left >= val) { used.push_back(idx); left -= val; }
        if (left == 0) break;
    }
    if (left != 0) return {false, 0, {}, "Exact sum not possible with available vouchers"};
    return {true, amount, std::move(used), "OK"};
}

// Redeem: picks exact set, marks as spent, updates balance.
inline RedemptionResult redeem_amount(HouseholdVouchersV2& hv, int merchant_id, int amount) {
    if (amount > hv.balance) return {false, 0, {}, "Insufficient balance"};
    auto pick = pick_exact(hv, amount);
    if (!pick.ok) return pick;

    for (int idx : pick.used_indices) hv.vouchers[idx].spent = true;
    hv.balance = compute_unspent_balance(hv);
    // (merchant_id could be logged in a real system)
    return pick;
}

// ======================== Utilities ========================
inline int get_balance(const HouseholdVouchersV2& hv) { return hv.balance; }
inline int get_issued_total(const HouseholdVouchersV2& hv) { return hv.issued_amount; }

inline void print_unspent(const string& tag, const HouseholdVouchersV2& hv) {
    cout << tag << " | balance=$" << hv.balance << " | vouchers: ";
    for (const auto& v : hv.vouchers) if (!v.spent) cout << '$' << to_int(v.value) << ' ';
    cout << '\n';
}

*/

// THEN 

/* void q3_6_demo() {
    // Households
    Household h1{1001, "Pioneer"};
    Household h2{1002, "Boon Lay"};
    HouseholdVouchersV2 hv1{h1.id}, hv2{h2.id};

    // Standard $50 pack: 5×$2, 4×$5, 2×$10
    auto pack50 = make_pack(0, 5, 4, 2);
    /*$1*/ /*$2*/ /*$5*/ /*$10*/
    
    // ADMIN issues
    //cout << "Issue hv1: " << (issue_voucher_pack(hv1, pack50) ? "ok" : "fail") << '\n';
    // cout << "Issue hv2: " << (issue_voucher_pack(hv2, pack50) ? "ok" : "fail") << '\n';
    // cout << "hv1 issued_total=$" << get_issued_total(hv1)
    //     << " | balance=$" << get_balance(hv1) << "\n";
    // print_unspent("hv1 before redemption", hv1);

    // MERCHANT redeems $20 from hv1
    // auto r = redeem_amount(hv1, /*merchant_id*/9001, /*amount*/20);
    /* if (r.ok) {
        cout << "Redeemed $" << r.redeemed_amount << " using indices: ";
        for (int i : r.used_indices) cout << i << ' ';
        cout << "\n";
    } else {
        cout << "Redeem failed: " << r.message << "\n";
    } */ 
    /*
    print_unspent("hv1 after $20 redemption", hv1);

    // Try a tricky amount (e.g., $3) when you don’t have $1 vouchers
    auto r2 = redeem_amount(hv1, 9001, 3);
    if (!r2.ok) cout << "Redeem $3 failed as expected: " << r2.message << "\n";
}*/

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




int main() {
    cout << "Hello, This is for Assignment 1" << '\n';
    q1_1();
    cout << '\n';
    q1_2();
    cout << '\n';
    q3_1();
    cout << '\n';
    q4_1();
    cout << "\n\nHello, This is for Assignment 2" << '\n';
    q2_1();
    cout << '\n';
    q2_2();
    cout << '\n';
    q2_3();
    cout << '\n';
    q2_4();
    cout << '\n';
    cout << "\n Hello, This is for Assignment 3" << '\n';
    // q3_1
  cout << "This is for question 3-1\n";
  for (double x = 0.0; x <= 1.0; x += 0.1) {
    cout << "x=" << setw(5) << fixed << setprecision(1) << x
         << "   f(x)=" << setw(8) << fixed << setprecision(5) << f(x) << endl;
  }
  cout << endl;
  // q3_2
  cout << "This is for question 3-2\n";
  q3_2();
  cout << endl;
  // q3_3
  cout << "\nThis is for question 3-3\n";
  cout << fixed << setprecision(5);
  bool verify_in_range = true;

  auto v = rand_vector(6); // [0,1]
  cout << "Random vector with 6 elements in range [0.0,1.0]:\n";
  for (double x : v) {
    cout << x << " ";
    if (x < 0.0 || x > 1.0)
      verify_in_range = false;
  }
  cout << endl;

  auto v1 = rand_vector(7, -1.0, 1.0); // [-1,1]
  cout << "Random vector with 7 elements in range [-1.0,1.0]:\n";
  for (double x : v1) {
    cout << x << " ";
    if (x < -1.0 || x > 1.0)
      verify_in_range = false;
  }
  cout << endl;

  auto v2 = rand_vector(8, 10.0, 20.0); // [10,20]
  cout << "Random vector with 8 elements in range [10.0,20.0]:\n";
  for (double x : v2) {
    cout << x << " ";
    if (x < 10.0 || x > 20.0)
      verify_in_range = false;
  }
  cout << endl;

  cout << (verify_in_range ? "All numbers are within desired range.\n"
                           : "Some numbers are out of range!\n");

  // q3_4
  cout << "\nThis is for question 3-4\n";
  int y = 7; 
  cout << "Initial value y is " << y << endl;
  change_x(y); // has no effect
  cout << "After change_x(y), y is " << y << endl;
  change_x_with_ref(y); // increases y by 1
  cout << "After change_x_with_ref(y), y is " << y << endl; // y is now 8 because it was increased by 1 by reference
  cout << "Function call change_x(y) has no effect, while change_x_with_ref(y) "
          "increases y by 1.\n"; // Explanation

  vector<int> vec(1'000'000, 1);
  auto start = chrono::high_resolution_clock::now();
  for (int i = 0; i < 1000; i++)
    value_copy(vec);
  auto end = chrono::high_resolution_clock::now();
  auto dur = chrono::duration_cast<chrono::milliseconds>(end - start);

  auto start1 = chrono::high_resolution_clock::now();
  for (int i = 0; i < 1000; i++)
    value_copy_with_ref(vec);
  auto end1 = chrono::high_resolution_clock::now();
  auto dur1 = chrono::duration_cast<chrono::milliseconds>(end1 - start1);

  cout << "Time taken for value_copy: " << dur.count() << " ms\n";
  cout << "Time taken for value_copy_with_ref: " << dur1.count() << " ms\n";
  cout << "value_copy_with_ref is faster than value_copy\n"; //because it avoids copying the large vector

  // q3_5
  cout << "\nThis is for question 3-5\n"; 
  q3_5();
  cout << endl;

  //

  // q3_7
  cout << "\nThis is for question 3-7\n";
  RateCurve IC{{41082, 0.0},
               {41083, 0.3447205943771747},
               {41085, 0.3447205943771747},
               {41092, 0.368367772605730},
               {41116, 0.525570772180778},
               {41146, 0.589858326742378}};
  vector<int> dates;
  vector<double> rates;
  for (auto [d, r] : IC) { // unpack each tuple
    dates.push_back(d); // append to dates vector
    rates.push_back(r); // append to rates vector
  }
  cout << fixed;
  cout << "date=41079: " << interpolate(dates, rates, 41079) << "\n";
  cout << "date=41083: " << interpolate(dates, rates, 41083) << "\n";
  cout << "date=41089: " << interpolate(dates, rates, 41089) << "\n";
  cout << "date=41152: " << interpolate(dates, rates, 41152) << "\n";


  return 0;

}
