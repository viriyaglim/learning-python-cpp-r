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

struct Point {
    double x;
    double y;
};
// Linear search function to find a target Point in a vector of Points
bool linear_search(const std::vector<Point>& points, const Point& target) {
    for (const auto& point : points) {
        if (point.x == target.x && point.y == target.y) {
            return true; // Target point found
        }
    }
    return false; // Target point not found
}


//binary search 

bool binary_search(const std::vector<Point>& points, const Point& target) {
    auto it = std::lower_bound(points.begin(), points.end(), target, [](const Point& a, const Point& b) {
        return (a.x < b.x) || (a.x == b.x && a.y < b.y);
    });
    return it != points.end() && it->x == target.x && it->y == target.y;
}

//shortest distance

void shortest_distance(vector<int>&a){
    int shortest = 0;
    for (size_t i = 1; i < a.size(); ++i) {
        for(size_t j = i; j < a.size(); ++j){
            int distance = abs(a[i] - a[j]);
            if (distance < shortest || shortest == 0) {
                shortest = distance;
            }

        }
    }
}

//print vector
void print_vector(const vector<int>& a) {
    for (const auto& elem : a) {
        cout << elem << " ";
    }
    cout << endl;
}


array<int, 3> arr{};
array<int, 3> arr2 = {1,2,3};
//bubble sort 
void bubble_sort(vector<int>& a) {
    bool swapped;
    do {
        swapped = false;
        for (size_t i = 1; i < a.size(); ++i) {
            if (a[i - 1] > a[i]) {
                std::swap(a[i - 1], a[i]);
                swapped = true;
            }
        }
    } while (swapped);
}

//quicksort
void quicksort(vector<int>& a, int low, int high) {
    if (low < high) {
        int pivot = a[high];
        int i = low - 1;
        for (int j = low; j < high; ++j) {
            if (a[j] < pivot) {
                ++i;
                std::swap(a[i], a[j]);
            }
        }
        std::swap(a[i + 1], a[high]);
        int pi = i + 1;
        quicksort(a, low, pi - 1);
        quicksort(a, pi + 1, high);
    }
}

//seeding random number generator
 seed_seq seed2{90128};
 auto mtgen = mt19937{seed2};
 void simulate() {
 auto nd = normal_distribution<>{5.0, 2.0};
 for (size_t i = 0; i < 10; ++i) {
    cout << nd(mtgen) << '\n'; // called 10 times here
 }
 }
 

int main(){
    std::vector<Point> points = {{1, 2}, {3, 4}, {5, 6}};
    Point target = {3, 4};

    if (linear_search(points, target)) {
        cout << "Linear search: Point found\n";
    } else {
        cout << "Linear search: Point not found\n";
    }

    if (binary_search(points, target)) {
        cout << "Binary search: Point found\n";
    } else {
        cout << "Binary search: Point not found\n";
    }

    vector<int> a = {5, 3, 8, 1, 2};
    print_vector(a);
    bubble_sort(a);
    print_vector(a);
    quicksort(a, 0, a.size() - 1);
    print_vector(a);

    set<string> visitors;
visitors.insert("Alice");
visitors.insert("Bob");
visitors.insert("Charlie");
visitors.erase("Bob");
cout << "Number of visitors: " << visitors.size() << endl;
cout << "Is Alice a visitor? " << (visitors.count("Alice") ? "Yes" : "No") << endl;
cout << "Is Bob a visitor? " << (visitors.count("Bob") ? "Yes" : "No") << endl; 

    set<int> numbers;
    for (size_t i = 0; i < 10; ++i) {
        numbers.insert(i);
    }
    numbers.erase(5);
    for (const auto& num : numbers) {
        cout << num << " ";
    }
    cout << endl;

    const auto it = numbers.find(5);
    if (it != numbers.end()) {
        cout << "Found: " << *it << endl;
    } else {
        cout << "Not found" << endl;
    }

    //map example
    map<char,int> my_map;
    my_map['a'] = 1;
    my_map['b'] = 2;
    my_map['c'] = 3;
    for (const auto& [key, value] : my_map) {
        cout << key << ": " << value << endl;
    }
    const map<char, int> second_map = { {'x', 24}, {'y', 25}, {'z', 26} };
    for (const auto& [key, value] : second_map) {
        cout << key << ": " << value << endl;
    }  

    

    array<int, 3> arr = {1,2,3};
    fill(arr.begin(), arr.end(), 0);
    for (const auto& elem : arr) {
        cout << elem << " ";
    }
    cout << endl;

    //iota

    array<int, 10> arr3;
    iota(arr3.begin(), arr3.end(), 1);
    for (const auto& elem : arr3) {
        cout << elem << " ";
    }
    cout << endl;

    //generate
    vector<int> vec(10);
    int cc = 0; 
    generate(vec.begin(), vec.end(), [&cc]() { return cc*cc; });
    for (const auto& elem : vec) {
        cout << elem << " ";
    }
    
    //all_of
    vector<int> vec2 = {2, 4, 6, 8};
    bool all_even = all_of(vec2.begin(), vec2.end(), [](int n) { return n % 2 == 0; });
    cout << "\nAll even: " << (all_even ? "Yes" : "No") << endl;

    // max and min element, accumulate, reduce, reverse
    vector<int> vec3 = {1, 2, 3, 4, 5};
    auto max_it = max_element(vec3.begin(), vec3.end());
    auto min_it = min_element(vec3.begin(), vec3.end());
    int sum = accumulate(vec3.begin(), vec3.end(), 0);
    int product = std::reduce(vec3.begin(), vec3.end(), 1, std::multiplies<int>());
    reverse(vec3.begin(), vec3.end());

    //for each 
    for_each(vec3.begin(), vec3.end(), [](int n) { cout << n << " "; });
    cout << "\nMax: " << *max_it << ", Min: " << *min_it << ", Sum: " << sum << ", Product: " << product << endl;

    //transform 
    vector<int> vec4 = {1, 2, 3, 4, 5};
    vector<int> vec5(vec4.size());
    transform(vec4.begin(), vec4.end(), vec5.begin(), [](int n) {
        return n * n;
    });

    for (const auto& elem : vec5) {
        cout << elem << " ";
    }

    //copy and copy_if
    vector<int> vec6 = {1, 2, 3, 4, 5};
    vector<int> vec7;
    copy(vec6.begin(), vec6.end(), back_inserter(vec7));
    vector<int> vec8; // to hold even numbers
    copy_if(vec6.begin(), vec6.end(), back_inserter(vec8), [](int n) { return n % 2 == 0; });

    //count and count_if
    int count_3 = count(vec6.begin(), vec6.end(), 3); // count occurrences of 3
    int count_even = count_if(vec6.begin(), vec6.end(), [](int n) { return n % 2 == 0; });
    cout << "\nCount of 3: " << count_3 << ", Count of even numbers: " << count_even << endl;
    

    //find and find_if
    auto find_4 = find(vec6.begin(), vec6.end(), 4);
    auto find_gt_3 = find_if(vec6.begin(), vec6.end(), [](int n) { return n > 3; });
    if (find_4 != vec6.end()) {
        cout << "Found 4 at position: " << distance(vec6.begin(), find_4) << endl;
    }

    if (find_gt_3 != vec6.end()) {
        cout << "First number greater than 3: " << *find_gt_3 << endl;
    }

    //sort
    vector<int> vec9 = {5, 2, 9, 1, 5, 6};
    sort(vec9.begin(), vec9.end());

    for (const auto& elem : vec9) {
        cout << elem << " ";
    }

    //sort for struct
    vector<Point> points2 = {{3, 4}, {1, 2}, {5, 6}};
    sort(points2.begin(), points2.end(), [](const Point& a, const Point&
    b) {
            return (a.x < b.x) || (a.x == b.x && a.y < b.y);
        });

   for (const auto& point : points2) {
       cout << "(" << point.x << ", " << point.y << ") ";
   }
   //binary search using STL

   vector<int> vec000 = {5,4,2,67,34,23,89,1};
    sort(vec000.begin(), vec000.end()); 
    int target000 = 34; 
    bool found000 = binary_search(vec000.begin(), vec000.end(), target000);
    cout << "\nBinary search for " << target000 << ": " << (found000 ? "Found" : "Not Found") << endl;

   //search for a sequence
    vector<int> vec10 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    vector<int> subseq = {4, 5, 6};
    auto it2 = search(vec10.begin(), vec10.end(), subseq.begin(), subseq.end());
    if (it2 != vec10.end()) {
        cout << "Subsequence found at position: " << distance(vec10.begin(), it2) << endl;
    } else {
        cout << "Subsequence not found" << endl;
    }

    //iomanip

    cout << "1) cout: " << '\n'
 << "Today's highest temperature is "
 << 35.0 << '\n'
 << "Today's highest temperature is "
 << setprecision(2) << fixed << 35.0
 << '\n'
 << "Today's highest temperature is "
 << scientific << 35.0 << '\n' << '\n';
 // reset the output format to default
 cout << defaultfloat;
 cout << "2) cout with setw:" << '\n'
 << "|" << setw(20) << "Name"
 << "|" << setw(20) << "Tel"
 << "|" << '\n'
 << "|" << setw(20) << "Daniel Yue"
 << "|" << setw(20) << "9xxxxxxxx" //setw only works for next output, give 20 spaces
 << "|" << '\n';

//Write the “Hello World” and numbers 1,⋯,10 and to the file test.txt.
 ofstream out("test.txt");
 out << "Hello World" << endl;
 for (int i = 1; i <= 10; ++i) {
     out << i << " ";
 }
 out.close();

    simulate();
    uniform_int_distribution<> ud(0, 1);
    cout << ud(mtgen) << '\n';
 
    return 0;
}