#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <memory> // smart pointers
#include <thread>
#include <mutex>
#include <algorithm>
#include <cassert>
#include <iomanip> // for std::setw, std::setprecision
#include <random> // for random number generation
#include <cmath> // for math functions
#include <functional> // for std::function
#include <utility> // for std::move, std::forward
#include <stdexcept> // for exceptions


using namespace std;
using namespace std::chrono;

class Matrix {
private:
    size_t rows_, cols_;
    vector<double> data_; // row-major: index = r*cols_ + c
public:
    // --- Constructors / Destructor ---
    Matrix() : rows_(0), cols_(0) {}

    Matrix(size_t r, size_t c, double val = 0.0)
        : rows_(r), cols_(c), data_(r * c, val) {}

    // Copy & move semantics (defaulted are fine here; vector handles memory)
    Matrix(const Matrix&) = default;
    Matrix(Matrix&&) noexcept = default;
    Matrix& operator=(const Matrix&) = default;
    Matrix& operator=(Matrix&&) noexcept = default;

    ~Matrix() = default; // RAII via std::vector (no manual delete needed)

    // --- Accessors ---
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }
    size_t size() const { return data_.size(); }

    // --- Element access ---
    // 2D access: m(r,c)
    double& operator()(size_t r, size_t c) {
        assert(r < rows_ && c < cols_);
        return data_[r * cols_ + c];
    }
    double operator()(size_t r, size_t c) const {
        assert(r < rows_ && c < cols_);
        return data_[r * cols_ + c];
    }

    // 1D access: m[i] (linear index). Useful for tight loops / interop.
    double& operator[](size_t idx) {
        assert(idx < data_.size());
        return data_[idx];
    }
    double operator[](size_t idx) const {
        assert(idx < data_.size());
        return data_[idx];
    }

    // --- Operator + (matrix addition) ---
    Matrix operator+(const Matrix& other) const {
        assert(rows_ == other.rows_ && cols_ == other.cols_);
        Matrix out(rows_, cols_);
        for (size_t i = 0; i < data_.size(); ++i) {
            out.data_[i] = data_[i] + other.data_[i];
        }
        return out; // NRVO / move elision
    }

    // --- Utility: fill & identity ---
    static Matrix identity(size_t n) {
        Matrix I(n, n, 0.0);
        for (size_t i = 0; i < n; ++i) I(i, i) = 1.0;
        return I;
    }

    // --- Pretty print ---
    void print(const string& title = "") const {
        if (!title.empty()) cout << title << "\n";
        for (size_t r = 0; r < rows_; ++r) {
            for (size_t c = 0; c < cols_; ++c) {
                cout << setw(8) << operator()(r, c) << ' ';
            }
            cout << '\n';
        }
        cout << '\n';
    }
};

// ==============================
// Abstract Base Class: Option
// ==============================
class Option {
protected:
    double S0;   // spot price
    double K;    // strike
    double r;    // risk-free rate
    double T;    // maturity
public:
    Option(double S0_, double K_, double r_, double T_)
        : S0(S0_), K(K_), r(r_), T(T_) {}
    virtual ~Option() = default;

    // Pure virtual method: must be implemented by derived classes
    virtual double price() const = 0;

    // Virtual: could be overridden if needed
    virtual string type() const { return "Generic Option"; }
};

// ==============================
// European Call Option (Black–Scholes closed-form)
// ==============================
class EuropeanCall : public Option {
private:
    double sigma; // volatility
    double N(double x) const { return 0.5 * erfc(-x / sqrt(2)); } // CDF Normal
public:
    EuropeanCall(double S0_, double K_, double r_, double T_, double sigma_)
        : Option(S0_, K_, r_, T_), sigma(sigma_) {}

    double price() const override {
        double d1 = (log(S0/K) + (r + 0.5*sigma*sigma)*T) / (sigma*sqrt(T));
        double d2 = d1 - sigma*sqrt(T);
        return S0 * N(d1) - K * exp(-r*T) * N(d2);
    }

    string type() const override { return "European Call"; }
};

// ==============================
// Asian Call Option (Monte Carlo average payoff)
// ==============================
class AsianCall : public Option {
private:
    double sigma; // volatility
    int nPaths;
    int nSteps;
public:
    AsianCall(double S0_, double K_, double r_, double T_, double sigma_,
              int nPaths_=10000, int nSteps_=100)
        : Option(S0_, K_, r_, T_), sigma(sigma_), nPaths(nPaths_), nSteps(nSteps_) {}

    double price() const override {
        mt19937 rng(42);
        normal_distribution<double> Z(0.0, 1.0);
        double dt = T / nSteps;
        double sumPayoff = 0.0;

        for (int i = 0; i < nPaths; i++) {
            double S = S0;
            double sumS = 0.0;
            for (int j = 0; j < nSteps; j++) {
                double dW = sqrt(dt) * Z(rng);
                S *= exp((r - 0.5*sigma*sigma)*dt + sigma*dW);
                sumS += S;
            }
            double avgS = sumS / nSteps;
            sumPayoff += max(avgS - K, 0.0);
        }
        return exp(-r*T) * (sumPayoff / nPaths);
    }

    string type() const override { return "Asian Call (Monte Carlo)"; }
};

// ==============================
// Generic Matrix Template
// ==============================
template <typename T>
class GenericMatrix {
private:
    size_t rows_, cols_;
    vector<T> data_;  // row-major order

public:
    // Constructor
    GenericMatrix(size_t r, size_t c, T val = T())
        : rows_(r), cols_(c), data_(r * c, val) {}

    // Accessors
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }

    // Element access via (r,c)
    T& operator()(size_t r, size_t c) {
        assert(r < rows_ && c < cols_);
        return data_[r * cols_ + c];
    }
    const T& operator()(size_t r, size_t c) const {
        assert(r < rows_ && c < cols_);
        return data_[r * cols_ + c];
    }

    // Matrix addition
    GenericMatrix<T> operator+(const GenericMatrix<T>& other) const {
        assert(rows_ == other.rows_ && cols_ == other.cols_);
        GenericMatrix<T> result(rows_, cols_);
        for (size_t i = 0; i < data_.size(); ++i) {
            result.data_[i] = data_[i] + other.data_[i];
        }
        return result;
    }

    // Scalar multiplication
    GenericMatrix<T> operator*(const T& scalar) const {
        GenericMatrix<T> result(rows_, cols_);
        for (size_t i = 0; i < data_.size(); ++i) {
            result.data_[i] = data_[i] * scalar;
        }
        return result;
    }

    // Print
    void print(const string& title = "") const {
        if (!title.empty()) cout << title << "\n";
        for (size_t r = 0; r < rows_; r++) {
            for (size_t c = 0; c < cols_; c++) {
                cout << setw(8) << operator()(r, c) << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }
};

// ==============================
// Template Function Example
// ==============================
template <typename T>
T add_generic(T a, T b) {
    return a + b;
}


// ==============================
// Demo Class with Move Semantics
// ==============================
class DataBuffer {
private:
    size_t size_;
    double* data_;
public:
    // Constructor
    DataBuffer(size_t n, double val = 0.0) : size_(n) {
        data_ = new double[size_];
        for (size_t i = 0; i < size_; i++) data_[i] = val;
        cout << "Constructed buffer of size " << size_ << endl;
    }

    // Destructor
    ~DataBuffer() {
        delete[] data_;
        cout << "Destroyed buffer of size " << size_ << endl;
    }

    // Copy Constructor
    DataBuffer(const DataBuffer& other) : size_(other.size_) {
        data_ = new double[size_];
        for (size_t i = 0; i < size_; i++) data_[i] = other.data_[i];
        cout << "Copied buffer of size " << size_ << endl;
    }

    // Copy Assignment
    DataBuffer& operator=(const DataBuffer& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            data_ = new double[size_];
            for (size_t i = 0; i < size_; i++) data_[i] = other.data_[i];
            cout << "Copy-assigned buffer of size " << size_ << endl;
        }
        return *this;
    }

    // Move Constructor
    DataBuffer(DataBuffer&& other) noexcept : size_(other.size_), data_(other.data_) {
        other.size_ = 0;
        other.data_ = nullptr;
        cout << "Moved buffer (constructor)" << endl;
    }

    // Move Assignment
    DataBuffer& operator=(DataBuffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            data_ = other.data_;
            other.size_ = 0;
            other.data_ = nullptr;
            cout << "Moved buffer (assignment)" << endl;
        }
        return *this;
    }

    // Print a few values
    void print() const {
        cout << "Buffer first element = " << (size_ > 0 ? data_[0] : 0) << endl;
    }
};

// ==============================
// Perfect Forwarding Example
// ==============================
// ...existing code...
template<typename T, typename... Args>
std::unique_ptr<T> make_object(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}




int main() {
    std::cout << "Hello, World!" << std::endl;
    cout << "=== C++ Quant Study Plan ===" << endl;

    // =======================
    // Week 1: Performance Foundations & OOP
    // =======================

    // ----- Day 1: Setup & Basics -----
    auto start = high_resolution_clock::now();

    cout << "[Day 1] Hello Quant World! 🚀" << endl;

    // Example: simple loop
    double sum = 0.0;
    for (int i = 0; i < 1e7; i++) {
        sum += i * 0.000001;
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    cout << "[Day 1] Sum = " << sum << endl;
    cout << "[Day 1] Execution time: " << duration.count() << " ms" << endl;
    // ----- End of Day 1 -----
    // =======================


    // ----- Day 2: Memory Model -----
    cout << "\n[Day 2] Memory Model" << endl;

    // 1. Stack vs Heap
    int stack_var = 42; // lives on stack
    int* heap_var = new int(99); // allocated on heap
    cout << "[Day 2] Stack var = " << stack_var << endl;
    cout << "[Day 2] Heap var = " << *heap_var << endl;
    delete heap_var; // manual cleanup required

    // 2. References vs Pointers
    int x = 10;
    int& ref = x;  // reference (alias to x)
    int* ptr = &x; // pointer (holds address of x)

    cout << "[Day 2] Reference value = " << ref << endl;
    cout << "[Day 2] Pointer value = " << *ptr << " (address: " << ptr << ")" << endl;

    ref = 20;   // modifies x
    *ptr = 30;  // modifies x again
    cout << "[Day 2] After modifications, x = " << x << endl;

    // 3. RAII & Smart Pointers
    {
        unique_ptr<int> uptr = make_unique<int>(123);
        cout << "[Day 2] unique_ptr value = " << *uptr << endl;

        shared_ptr<int> sptr1 = make_shared<int>(456);
        shared_ptr<int> sptr2 = sptr1; // shared ownership
        cout << "[Day 2] shared_ptr values = " << *sptr1 << ", " << *sptr2 << endl;
        cout << "[Day 2] shared_ptr use_count = " << sptr1.use_count() << endl;
    } // automatic cleanup here (RAII principle)
    // Do not reuse 'ptr' for int* later, to avoid confusion.
    // ----- End of Day 2 -----

    // =======================
    cout << "[Day 3] OOP Core: Classes, Ctors/Dtors, Operator Overloading (+, [], ())\n\n";

    // Construct 2x3 matrices
    Matrix A(2, 3, 1.0);   // all ones
    Matrix B(2, 3, 2.5);   // all 2.5

    // Modify elements via (r,c)
    A(0, 1) = 5.0;         // set A[0,1] = 5
    A(1, 2) = -3.0;

    // Modify elements via linear [] (row-major)
    // index = r*cols + c -> for 2x3, A[1,0] is index 1*3+0 = 3
    A[3] = 9.0;            // sets element (1,0)

    A.print("Matrix A:");
    B.print("Matrix B:");

    // Matrix addition
    Matrix C = A + B;
    C.print("C = A + B:");

    // Identity example
    Matrix I = Matrix::identity(3);
    I.print("I3 (identity):");

    // Read elements
    cout << "C(1,2) = " << C(1, 2) << "\n";
    cout << "A linear [3] = " << A[3] << " (i.e., A(1,0))\n";

    cout << "\n[Day 3] Done ✅\n";
    // =======================
    cout << "[Day 4] Inheritance & Polymorphism\n\n";

    // Use polymorphism: base class pointer to derived object
    unique_ptr<Option> opt1 = make_unique<EuropeanCall>(100, 100, 0.05, 1.0, 0.2);
    unique_ptr<Option> opt2 = make_unique<AsianCall>(100, 100, 0.05, 1.0, 0.2);

    cout << opt1->type() << " Price = " << opt1->price() << endl;
    cout << opt2->type() << " Price = " << opt2->price() << endl;

    cout << "\n[Day 4] Done ✅\n";

    // =======================
        cout << "[Day 5] Templates & Generics\n\n";

    // Using generic add function
    cout << "add_generic<int>(7, 8) = " << add_generic<int>(7, 8) << endl;
    cout << "add_generic<double>(1.1, 2.2) = " << add_generic<double>(1.1, 2.2) << endl;

    // GenericMatrix<int>
    GenericMatrix<int> M1(2, 2, 3);
    GenericMatrix<int> M2(2, 2, 4);
    GenericMatrix<int> M3 = M1 + M2;
    M3.print("GenericMatrix<int> M3 = M1 + M2:");

    // GenericMatrix<double>
    GenericMatrix<double> N1(2, 2, 1.25);
    GenericMatrix<double> N2(2, 2, 0.75);
    GenericMatrix<double> N3 = N1 * 2.5;
    N3.print("GenericMatrix<double> N3 = N1 * 2.5:");

    cout << "\n[Day 5] Done ✅\n";
    // =======================
    cout << "[Day 6] Move Semantics & Copy Elision\n\n";

    // Normal construction
    DataBuffer buf1(1000000, 1.23);  // big buffer

    // Copy (expensive)
    DataBuffer buf2 = buf1;          // invokes copy constructor

    // Move (cheap, just steals pointer)
    DataBuffer buf3 = std::move(buf1); // invokes move constructor

    buf2.print();
    buf3.print();
    // Perfect forwarding (factory)
    auto db_ptr = make_object<DataBuffer>(500000, 4.56);
    db_ptr->print();
    cout << "\n[Day 6] Done ✅\n";
    // =======================
    // =======================

    return 0;

}
