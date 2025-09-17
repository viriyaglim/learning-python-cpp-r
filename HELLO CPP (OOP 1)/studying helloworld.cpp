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

using namespace std;
using std::cout;
using namespace std::string_literals; // <-- enables "..."s literals

void test_value_copy(vector<int> x) {return ;} // pass by value (copy)

double payoff_call(double S, double K, double r, double T, double sigma) {
        auto d1 = (log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * sqrt(T));
        auto d2 = d1 - sigma * sqrt(T);
        auto N = [](double x) { return 0.5 * erfc(-x * 0.7071067811865476); }; // CDF of standard normal
        return S * N(d1) - K * exp(-r * T) * N(d2);
    }

inline int add(int a, int b) { return a + b; }

//example factorial
    int factorial(int n) {
        if (n <= 1) return 1;
        return n * factorial(n - 1);
    }


//long long fibonacci
    long long fibonacci(int n) {
        if (n <= 1) return n;
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
//
unsigned long long fib_linear(int n) {
vector<unsigned long long> fibonacci(n + 1, 1);
for (size_t i = 2; i <= n; ++i) {
  fibonacci[i] = fibonacci[i - 1] + fibonacci[i - 2];
 }
 return fibonacci.back();
}

//tuple example 
tuple <int, string, double> get_data() {
    return {42, "Hello", 3.14};
}
auto get_tuple2() {
    return make_tuple(1, "World", 2.71);
}

//random number example
random_device rd;
auto mtgen = mt19937{rd()}; //Mersenne Twister generator

void some_func() {
    auto urdd = uniform_real_distribution<double>{0.0, 1.0};
}

double add_up_vector(const vector<double>& v) { //& is for pass-by-reference
    double sum = 0.0;
    for (const auto& e : v) sum += e;
    return sum;
}
int main() {
    int x = 3;
    cout << x << '\n';

    string greeting = "Hello World!";
    cout << greeting << '\n';

    auto greeting1 = "Hello World!"s; // now valid
    cout << greeting1 << '\n';

    vector<int> v{1,2,3};
    {
        const int a{3};
        const int b{4};
        const int sum{a+b};
        cout << sum << '\n';
    }

    int x2{};              // 0
    int y{x2};             // 0
    unsigned int ux{3};    // 3
    bool z{true};          // 1
    double d{2.4};         // 2.4
    string s{"var"};       // var
    cout << x2 << ' ' << y << ' ' << ux << ' ' << z << ' ' << d << ' ' << s << '\n';

    vector<int> vec{1,2,3,4,5};
    for (const auto& e : vec) cout << e << ' ';
    cout << '\n';

    vector<double> vec_1{2,0};
    vector<double> vec_2{3,0};
    vector<vector<double>> vec_3{vec_1, vec_2};

    auto vec_3x3 = vector<vector<double>>{
        {1,2,3},
        {4,5,6},
        {7,8,9}
    };

    auto vec_3x2_with_2 = vector<vector<double>>(3, vector<double>(2, 2));
    for (const auto& row : vec_3x2_with_2) {
        for (const auto& elem : row) cout << elem << ' ';
        cout << '\n';
    }

    vector<vector<int>> aInt2x4 = {
        {1,2,3,4},
        {5,6,7,8}
    };
    cout << aInt2x4[1][2] << '\n'; // 7

    const string password(16, 'x');
    cout << password << "\n";

    auto s0 = "abc"s;
    string s1{};
    s1 = "Today is Saturday";
    const string s2{s1};
    cout << s2 << '\n';

    const auto s1_concat{s1 + s1};
    const auto s1_sub{s1.substr(3,10)};
    auto s1_lower(s1);
    s1_lower[0] = static_cast<char>(std::tolower(static_cast<unsigned char>(s1_lower[0]))); // safe tolower
    cout << s1_concat << "\n" << s1_sub << "\n" << s1_lower << '\n';

    const auto i1 = stoi("42");
    const auto i2 = stoll("1010010");
    const auto i3 = stod("122222.2");
    (void)i1; (void)i2; (void)i3;

    struct TypeName {
        int id;
        string name;
        double value;
    };

    TypeName obj{1, "Sample", 3.14};
    cout << obj.id << ' ' << obj.name << ' ' << obj.value << '\n';
    TypeName obj2 = {2, "Example", 2.71};
    cout << obj2.id << ' ' << obj2.name << ' ' << obj2.value << '\n';

    struct Ticket {
        int id;
        string event;
        double price;
    };
    Ticket tk{1, "Concert", 99.99};
    Ticket tk2{2, "Theater", 49.99};

    auto [tk_id, tk_event, tk_price] = tk;   // needs C++17+
    cout << tk_id << ' ' << tk_event << ' ' << tk_price << '\n';
    auto [tk2_id, tk2_event, tk2_price] = tk2;
    cout << tk2_id << ' ' << tk2_event << ' ' << tk2_price << '\n';

    int xx = 42;
    int yyy{xx};
    cout << (xx + yyy) << '\n';
    cout << (xx = xx + 100) << '\n';
    xx += 100;
    cout << xx << '\n';

    int a = 42;
    int bb = ++a; // 43
    int cc = 20;
    int bbb = cc++; // 20 then cc=21
    cout << a << ' ' << bb << ' ' << cc << ' ' << bbb << '\n';

    // boolean expressions (no output, just evaluated)
    (void)(((1<=1) || (0==0)) && ((5<5) || (5!=5)));
    (void)((1<=1) || ((0==0) && ((5<5) && (5!=5))));

    double pi = 3.14159265358979323846;
    double result = 4 < 5 ? sin(pi/6) : cos(pi/6);
    cout << result << '\n';

    if (0) cout << "This won't print.\n"; else cout << "This will print.\n";
    if (1) cout << "This will print.\n"; else cout << "This won't print.\n";
    if (-1) cout << "This will print.\n"; else cout << "This won't print.\n";

    for (size_t i = 0; i < 5; ++i) cout << i << ' ';
    cout << '\n';

    int temp_sum = 0;
    for (size_t i = 0; i < 10; ++i) {
        temp_sum += static_cast<int>(i);
        cout << temp_sum << ' ';
    }
    cout << '\n';

    int sum = 0;
    for (size_t n = 1; n <= 100; ++n) {
        sum += static_cast<int>(n * n);
        cout << sum << ' ';
    }
    cout << '\n';

    {
        int x{3072};
        while (x % 2 == 0) {
            x /= 2;
            cout << x << ' ';
        }
        cout << '\n';
    }

    {
        for (size_t i = 0; i < 10; ++i) {
            if (i == 5) continue;
            cout << i << ' ';
        }
        cout << '\n';
    }

    cout << add_up_vector({1.1, 2.2, 3.3, 4.4, 5.5}) << '\n';

    { //reference lesson
        int x(3), y(4);
        int &r = x; // r is a reference to x, r = x
        r = 5;      // x = 5
        cout << x << ' ' << r << '\n'; // 5 5

    }

    cout << add(3,4) << '\n';

    //counting time
    vector<int> large_vec(1'000'000, 1);
    auto start = chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < 100; ++i) {
        test_value_copy(large_vec); // pass by value (copy)
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    cout << "Duration: " << duration.count() << " ms\n";
    

    cout << payoff_call(100, 100, 0.05, 1, 0.2) << '\n';
    constexpr double M_SQRT1_2 = 0.70710678118654752440; // 1/sqrt(2)

    //lambda function from function above
    auto payoff_call_lambda = [](double S, double K, double r, double T, double sigma) {
        auto d1 = (log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * sqrt(T));
        auto d2 = d1 - sigma * sqrt(T);
        auto N = [](double x) { return 0.5 * erfc(-x * M_SQRT1_2); }; // CDF of standard normal
        return S * N(d1) - K * exp(-r * T) * N(d2);
    };
    cout << payoff_call_lambda(100, 100, 0.05, 1, 0.2) << '\n';

    //capture clause
    double factor = 1.5;
    auto multiply = [factor](double x) { return x * factor; }; // capture factor by value, result is 1.5 * x
    cout << multiply(10) << '\n'; // 15
    // if we use &factor, it captures by reference, result is the same
    // if we use =, it captures all local variables by value, result is the same

    //how it differs, if we use & in the capture clause, it captures the variable by reference
    // this means that if the variable is modified outside the lambda, the lambda will see the updated value
    // if we use =, it captures all local variables by value, so the lambda will not see any changes made to the variables outside the lambda
    cout << factorial(5) << '\n'; // 120
    cout << fibonacci(10) << '\n'; // 55
    cout << fib_linear(50) << '\n'; // 12586269025



    return 0;
}
