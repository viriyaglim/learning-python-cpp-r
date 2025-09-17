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

//q5_1
void intersectionset(const set<int>& a, const set<int>& b, const string& name ) {
    vector<int> intersection; // to hold the intersection result
    set_intersection(a.begin(), a.end(), b.begin(), b.end(), back_inserter(intersection)); // compute intersection
// back inserter to append results to the vector
    cout << name << ": " << intersection.size(); 
    if(!intersection.empty()){
        cout << " [ ";
        for (int x: intersection) { // print each element in the intersection
            cout << x << " ";
        }
        cout << "]";
    }
    cout << endl;
}
void q5_1(){
    set<int> set1{1,2,3,4,5};
    set<int> set2{2,3,4,5,7};
    set<int> set3{3,8,9,7,6};
    
    intersectionset(set1, set2, "set 1 and set 2");
    intersectionset(set1, set3, "set 1 and set 3");
    intersectionset(set2, set3, "set 2 and set 3");

}

//q5_2
void q5_2(){
    string word_list = "Apple Banana Strawberry Tomato Elephant Umbrella Vanilla House Ice Jaguar";
    istringstream iss(word_list); // input string stream

    vector<string> words; // to hold individual words
    string word;

    while (iss >> word){ //check if we can extract a word
        words.push_back(word); // extract words from the stream
    }

    vector<int> numbers = {8, 2, 4, 6, 7, 1, 9, 5, 3, 10, 2, 7, 1, 6, 8};
    cout << "Result sequence is ";
    for (int num : numbers){
        cout << words[num-1] << " ";
    }
    cout << endl;
}

//q5_3
// ---- Types ----
using RateCurve = vector<tuple<int,double>>; 
constexpr double DAYS_IN_YEAR = 365.0;

inline double years_to_maturity(int today, int date) {
    double days = double(date - today);
    return max(0.0, days) / DAYS_IN_YEAR;
}

// 1) Parallel shock: r' = r + delta  (delta e.g. ±0.02 = ±20 bps)
RateCurve parallel_shock(double delta, int /*today*/, const RateCurve& IC) {
    RateCurve out(IC.size()); // to hold the shocked curve
    transform(IC.begin(), IC.end(), out.begin(),
              [=](const auto& pt){
                  auto [date, r] = pt;
                  return make_tuple(date, r + delta); // apply parallel shock
              });
    return out;
}

// 2) Short-rates shock (< 365 days): r' = r + delta * exp(-t/4),  t in years
RateCurve short_rates_shock(double delta, int today, const RateCurve& IC) {
    RateCurve out(IC.size()); // to hold the shocked curve
    transform(IC.begin(), IC.end(), out.begin(),
              [=](const auto& pt){
                  auto [date, r] = pt;
                  double t = years_to_maturity(today, date);
                  double shock = (double(date - today) < 365.0) ? delta * exp(-t/4.0) : 0.0;
                  return make_tuple(date, r + shock); // apply short-rates shock
              });
    return out;
}

// 3) Rotational shock (steepener/flattener):
//    Short  (<365d):  ±delta_short * exp(-t/4)       with delta_short = delta (argument)
//    Long   (≥365d):  ±delta_long  * (1 - exp(-t/4)) with delta_long  = 0.015  (spec)
// Pass +delta for steepener, -delta for flattener on the SHORT end.
// Change sign of delta_long below if you want the opposite on the LONG end.
RateCurve rotational_shock(double delta, int today, const RateCurve& IC) {
    const double delta_long = +0.015; // as in the prompt (flip sign for flattener effect)
    RateCurve out(IC.size()); // to hold the shocked curve
    transform(IC.begin(), IC.end(), out.begin(),
              [=](const auto& pt){
                  auto [date, r] = pt;
                  double t = years_to_maturity(today, date);
                  double e = exp(-t/4.0);
                  double shock = (double(date - today) < 365.0) ? (delta * e)
                                                                : (delta_long * (1.0 - e));
                  return make_tuple(date, r + shock); // apply rotational shock
              });
    return out;
}


void print_curve(const string& title, const RateCurve& c) {
    cout << title << "\n";
    for_each(c.begin(), c.end(), [](const auto& pt){
        cout << "  date=" << get<0>(pt) << "  rate%=" << get<1>(pt) << '\n';
    });
}

void q5_3() {
    // Sample input curve (date in days from today; rates in %)
    RateCurve IC = {
        {41082, 0.0},
                   {41083, 0.344720594377147},
                   {41085, 0.344720594377147},
                   {41092, 0.368368772605730},
                   {41116, 0.525570772180778},
                   {41146, 0.589858326742378},
                   {41179, 0.741119832110955},
                   {41188, 0.696749901029990},
                   {41278, 0.806659936532139},
                   {41368, 0.884475547507158},
                   {41461, 0.941966915221572},
                   {41552, 0.992016219922792},
                   {41649, 1.04351212131796},
                   {41816, 1.34312007757781},
                   {42181, 1.59973094475305},
                   {42546, 1.86320995459478},
                   {42911, 2.11609386013679},
                   {43279, 2.34400261226439},
                   {43643, 2.53599931753348},
                   {44007, 2.70067752825491},
                   {44372, 2.83822638195572},
                   {44738, 2.95830187616262},
                   {45470, 3.16075166306880},
                   {46564, 3.33878283257801},
                   {48390, 3.40297375743186},
                   {50216, 3.29434168467002},
                   {52043, 3.11704336212427}
    };
    int today = 0;

    // Choose signs per scenario
    auto par_up     = parallel_shock(+0.02,  today, IC); // +20 bps
    auto short_down = short_rates_shock(-0.03, today, IC); // -30 bps, fades with t for <365d
    auto rot_steep  = rotational_shock(+0.03, today, IC); // short up (+0.03*e^-t/4), long +0.015*(1-e^-t/4)

    print_curve("Parallel +20 bps:", par_up);
    print_curve("Short-rates -30 bps (only <1y, decays):", short_down);
    print_curve("Rotational (steepener):", rot_steep);
}

//q5_5
//q5_5 method 1 (given)
bool is_strictly_sorted_1(const std::vector<int>& v) {// method 1: using adjacent_find with greater_equal
 return std::adjacent_find(v.begin(), v.end(),
 std::greater_equal<int>()) == v.end(); 
}

//q5_5 method 2 (given)
bool is_strictly_sorted_2(const std::vector<int>& v) { // method 2: using all_of
 if (v.size() <= 1) return true;
 return std::all_of(v.begin(), v.end() - 1,
 [&v](const int& element) {
 size_t index = &element - &v[0];
 return v[index] < v[index + 1];
 });
 }

void print_case(const vector<int>&v, const string& label){ // print the test case and results
    cout << label << " [ ";
    for (int x : v) cout << x << ' ';
    cout << "] is strictly increasing? "
    << (is_strictly_sorted_1(v) ? "true" : "false")
    << " (by method 1), "
    << (is_strictly_sorted_2(v)? "true" : "false")
    << " (by method 2) \n";
}

void q5_4(){
    random_device rd; 
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0.0, 1.0);

    // Generate random bool vector of size 1,000,000
    vector<bool> random_bits(1'000'000); //random_bits for 1 million elements
    for (size_t i = 0; i < random_bits.size(); ++i) {
        random_bits[i] = (dis(gen) > 0.5); // true with probability 0.5
    }

    // Pattern: 15 consecutive true values
    vector<bool> pattern(15, true); // pattern of 15 trues

    // Search for the pattern
    auto result = search(random_bits.begin(), random_bits.end(),
                         pattern.begin(), pattern.end());

    // Print result
    if (result != random_bits.end()) {
        cout << "Pattern found at position: " // print the starting index
             << distance(random_bits.begin(), result) << endl;
    } else {
        cout << "Pattern not found." << endl;
    }
}

void q5_5(){
    vector<vector<int>> tests = {
        //based on the test cases (given)
        {},                         // empty
        {37},                       // single element
        {1, 2, 3, 4, 5},            // strictly increasing
        {1, 2, 2, 4, 5},            // not strictly increasing (equal adjacents)
        {5, 4, 3, 2, 1},            // decreasing
        {3, 1, 4, 1, 5}             // random order
    };
    vector<string> names = {
        "Empty",
        "Single",
        "Strictly increasing",
        "Non-strictly increasing",
        "Decreasing",
        "Random"

    };
    for (size_t i = 0; i < tests.size(); ++i){
        print_case(tests[i], names[i]);
    }
}

//q5_6
void q5_6() {
    int n = 21; // number of vertices

    // Step 1: Initialize lists L and component IDs C
    vector<list<int>> L(n + 1);  // L[i] = list of vertices in component i
    vector<int> C(n + 1);        // C[v] = current component ID of vertex v

    for (int i = 1; i <= n; ++i) {
        L[i].push_back(i);  // each vertex starts in its own list
        C[i] = i;           // each vertex starts in its own component
    }

    // Step 2: Hardcoded edges (same as in graph2.txt)
    vector<pair<int,int>> edges = {
        {1,2}, {2,11}, {11,12}, {12,13}, {13,15},
        {11,7}, {7,6}, {6,4}, {4,3}, {3,1},
        {4,5}, {14,16}, {8,10}, {9,8}, {19,20}, {20,21}
    };

    // Step 3: Process each edge
    for (auto [a, b] : edges) {
        if (C[a] != C[b]) { // only merge if different components
            int Ca = C[a];  // component ID of vertex a
            int Cb = C[b];  // component ID of vertex b

            // Reassign all vertices from component Ca to component Cb
            for (int v : L[Ca]) {
                C[v] = Cb;
            }

            // Move (splice) the whole list L[Ca] into L[Cb]
            L[Cb].splice(L[Cb].end(), L[Ca]);
        }
    }

    // Step 4: Print connected components
    cout << "Connected components:\n";
    for (int i = 1; i <= n; ++i) {
        if (!L[i].empty()) {
            cout << "{ ";
            for (int v : L[i]) cout << v << " ";
            cout << "}\n";
        }
    }
}

//q5_7
// Write a program that asks the user to type the coordinates of two points, A and B (in a plane), and then outputs the distance between A and B.
void q5_7(){
    double x1, y1, x2, y2;
    cout << "Enter coordinates of point A (x1 y1): ";
    cin >> x1 >> y1;
    cout << "Enter coordinates of point B (x2 y2): ";
    cin >> x2 >> y2;

    double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    cout << "Distance between A and B: " << distance << endl;
}

//q5_8
//1. Use the scientific notation in a C++ program to compute how much energy (in Joule) is contained in 70kg of matter, according to Einstein’‘s formula
//2. A human heartbeat requires roughly 1 Joule. Assuming one heartbeat per second, compute how many years the energy computed in part 1 can sustain a heartbeat 
//Hints: 𝐸 = 𝑚𝑐2, 𝑐 = 3𝑒8𝑚/𝑠2, where (in Joule) is the energy contained in the mass (in kg) and is the speed of light (in meters per second)
void q5_8(){
    const double mass = 70.0; // in kg
    const double speed_of_light = 3e8; // in m/s
    const double energy_per_heartbeat = 1.0; // in Joules
    const double seconds_per_year = 365.25 * 24 * 3600; // accounting for leap years

    double energy = mass * speed_of_light * speed_of_light; // E = mc^2
    double years_of_heartbeats = energy / energy_per_heartbeat / seconds_per_year;

    cout << "Energy contained in 70kg of matter: " << energy << " Joules\n";
    cout << "This energy can sustain a heartbeat for approximately: " << years_of_heartbeats << " years\n";
}

//q5_9
// Write a program that outputs the numbers 1,2, … ,100 and their square roots to a file SquareRoots.txt• The content of the file should look like this, use iomanip to adjust.
//1 1, 2 1.4142136

void q5_9(){
    ofstream out("SquareRoots.txt");
    if (!out) {
        cerr << "Error opening file for writing.\n";
        return;
    }
    out.precision(7);
    out << fixed; // fixed-point notation
    for (int i = 1; i <= 100; ++i) { // write number and its square root
        out << i << " " << sqrt(i) << "\n";
    }
    out.close(); // close the file
    cout << "Square roots written to SquareRoots.txt\n";
}


//q5_10
//ChatGPT really helped me doing this code, still understanding the code, it also recommended me 
//to use the uint64
void write_string(ofstream& out, const string& s) { 
    uint64_t n = s.size(); 
    out.write(reinterpret_cast<const char*>(&n), sizeof(n)); // write length
    out.write(s.data(), static_cast<std::streamsize>(n)); // write characters
}
void read_string(ifstream& in, string& s) {
    uint64_t n{}; 
    in.read(reinterpret_cast<char*>(&n), sizeof(n));
    s.resize(static_cast<size_t>(n)); // resize string to hold characters
    if (n) in.read(&s[0], static_cast<std::streamsize>(n)); // read characters
} 

void write_vector(ofstream& out, const vector<double>& v) {
    uint64_t n = v.size();
    out.write(reinterpret_cast<const char*>(&n), sizeof(n));
    if (n) out.write(reinterpret_cast<const char*>(v.data()),
                     static_cast<std::streamsize>(n * sizeof(double)));
}
void read_vector(ifstream& in, vector<double>& v) {
    uint64_t n{};
    in.read(reinterpret_cast<char*>(&n), sizeof(n));
    v.resize(static_cast<size_t>(n));
    if (n) in.read(reinterpret_cast<char*>(v.data()),
                   static_cast<std::streamsize>(n * sizeof(double)));
}

void write_list(ofstream& out, const list<int>& L) {
    uint64_t n = L.size();
    out.write(reinterpret_cast<const char*>(&n), sizeof(n));
    for (int x : L) {
        out.write(reinterpret_cast<const char*>(&x), sizeof(x));
    }
}

void read_list(ifstream& in, list<int>& L) {
    uint64_t n{};
    in.read(reinterpret_cast<char*>(&n), sizeof(n));
    L.clear();
    for (uint64_t i = 0; i < n; ++i) {
        int x{};
        in.read(reinterpret_cast<char*>(&x), sizeof(x));
        L.push_back(x);
    }
}



// data setup
struct Bond {
    string name;          
    list<int> schedule;   
    double price;         
    vector<double> cashflow; 
};

void q5_10() {
    // example 
    Bond b;
    b.name = "US 10Y Bond";
    for (int y = 2021; y <= 2030; ++y) b.schedule.push_back(y);
    b.price = 100.0;
    b.cashflow = vector<double>(10, 3.0);
    b.cashflow.back() = 103.0;

    {
        ofstream out("bond.bin", ios::binary);
        write_string(out, b.name);
        write_list(out, b.schedule);
        out.write(reinterpret_cast<const char*>(&b.price), sizeof(b.price));
        write_vector(out, b.cashflow);
    }

    Bond r;
    {
        ifstream in("bond.bin", ios::binary);
        read_string(in, r.name);
        read_list(in, r.schedule);
        in.read(reinterpret_cast<char*>(&r.price), sizeof(r.price));
        read_vector(in, r.cashflow);
    }

    cout << "Name: " << r.name << "\n";
    cout << "Schedule: ";
    for (int y : r.schedule) cout << y << ' ';
    cout << "\nPrice: " << r.price << "\nCashflows: ";
    for (double c : r.cashflow) cout << c << ' ';
    cout << "\n";
}

//q5_11
void q5_11(){
    const int N = 1000000;
    const int a = 1;
    const int b = 2000; 

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(a, b); //U(a,b)

    int count = 0; 
    for (int i = 0; i < N; i++){
        int num = dist(gen);
        
        if (num >=500 && num <= 1000 && num% 163 ==0){ // check the condition
            count++;
        }

    }
    cout << "From simulating " << N << " random numbers, "
    << count << " numbers are in range [500,1000] and divisible by 163.\n";


}
// 


int main(){
    cout << "This is for q5_1\n" << endl;
    q5_1();
    cout <<"\n";
    cout << "This is for q5_2\n" << endl;
    q5_2();
    cout <<"\n";
    cout << "This is for q5_3\n" << endl;
    q5_3();
    cout <<"\n";
    cout << "This is for q5_4\n" << endl;
    q5_4();
    cout <<"\n";
    cout << "This is for q5_5\n" << endl;
    q5_5();
    cout <<"\n";
    cout << "This is for q5_6\n" << endl;
    q5_6();
    cout <<"\n";
    cout << "This is for q5_7\n" << endl;
    q5_7();
    cout <<"\n";
    cout << "This is for q5_8\n" << endl;
    q5_8();
    cout <<"\n";
    cout << "This is for q5_9\n" << endl;
    q5_9();
    cout <<"\n";
    cout << "This is for q5_10\n" << endl;
    q5_10();
    cout <<"\n";
    cout << "This is for q5_11\n" << endl;
    q5_11();
    return 0; 

}

