#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <random>
#include <memory>
#include <stdexcept>
#include <list>
#include <cmath>
#include <iomanip>
#include <limits>
#include <corecrt_math_defines.h>
#include <cassert>



using namespace std;
using namespace std::chrono;

// ==============================
// Simple Memory Pool
// ==============================
class MemoryPool {
private:
    struct Block { Block* next; };
    Block* freeList = nullptr;
    vector<unique_ptr<char[]>> chunks; // keep ownership
    size_t blockSize;
    size_t blocksPerChunk;

    void allocateChunk() {
        auto chunk = make_unique<char[]>(blockSize * blocksPerChunk);
        char* start = chunk.get();

        for (size_t i = 0; i < blocksPerChunk; i++) {
            Block* block = reinterpret_cast<Block*>(start + i * blockSize);
            block->next = freeList;
            freeList = block;
        }
        chunks.push_back(move(chunk));
    }

public:
    MemoryPool(size_t blockSize_, size_t blocksPerChunk_ = 1024)
        : blockSize(max(blockSize_, sizeof(Block))),
          blocksPerChunk(blocksPerChunk_) {
        allocateChunk();
    }

    void* allocate() {
        if (!freeList) allocateChunk();
        Block* block = freeList;
        freeList = block->next;
        return block;
    }

    void deallocate(void* p) {
        Block* block = reinterpret_cast<Block*>(p);
        block->next = freeList;
        freeList = block;
    }
};

// ==============================
// Custom Allocator (STL-compatible)
// ==============================
template <typename T>
class PoolAllocator {
public:
    using value_type = T;

    MemoryPool* pool;

    PoolAllocator(MemoryPool* p = nullptr) noexcept : pool(p) {}

    template <typename U>
    PoolAllocator(const PoolAllocator<U>& other) noexcept : pool(other.pool) {}

    T* allocate(size_t n) {
        if (n != 1) throw bad_alloc(); // simple version: one object at a time
        return static_cast<T*>(pool->allocate());
    }

    void deallocate(T* p, size_t) noexcept {
        pool->deallocate(p);
    }
};

// Required for STL compatibility
template <typename T, typename U>
bool operator==(const PoolAllocator<T>& a, const PoolAllocator<U>& b) {
    return a.pool == b.pool;
}
template <typename T, typename U>
bool operator!=(const PoolAllocator<T>& a, const PoolAllocator<U>& b) {
    return !(a == b);
}

// ---------- Utilities: epsilon-based comparison ----------
bool approx_equal(double a, double b, double rel_eps = 1e-12, double abs_eps = 1e-15) {
    double diff = fabs(a - b);
    return diff <= max(abs_eps, rel_eps * max(fabs(a), fabs(b)));
}

// ---------- Kahan (compensated) summation ----------
double kahan_sum(const vector<double>& v) {
    double sum = 0.0, c = 0.0;
    for (double x : v) {
        double y = x - c;
        double t = sum + y;
        c = (t - sum) - y;
        sum = t;
    }
    return sum;
}

// ---------- Normal CDF (Phi) ----------
inline double norm_cdf(double x) {
    return 0.5 * erfc(-x / sqrt(2.0));
}

// ---------- Black–Scholes call price and vega ----------
double bs_call(double S, double K, double r, double T, double sigma) {
    if (sigma <= 0.0 || T <= 0.0) return max(0.0, S - K * exp(-r*T));
    double sqrtT = sqrt(T);
    double d1 = (log(S/K) + (r + 0.5*sigma*sigma)*T) / (sigma*sqrtT);
    double d2 = d1 - sigma*sqrtT;
    return S * norm_cdf(d1) - K * exp(-r*T) * norm_cdf(d2);
}

double bs_vega(double S, double K, double r, double T, double sigma) {
    if (sigma <= 0.0 || T <= 0.0) return 0.0;
    double sqrtT = sqrt(T);
    double d1 = (log(S/K) + (r + 0.5*sigma*sigma)*T) / (sigma*sqrtT);
    // pdf = exp(-0.5 d1^2)/sqrt(2π)
    double pdf = exp(-0.5*d1*d1) / sqrt(2.0*M_PI);
    return S * pdf * sqrtT;
}

// ---------- Newton–Raphson Implied Volatility ----------
struct IVResult { double sigma; int iters; bool ok; };

IVResult implied_vol_newton(double market_price,
                            double S, double K, double r, double T,
                            double init_sigma = 0.2,
                            int max_iter = 100,
                            double tol = 1e-10)
{
    double sigma = init_sigma;
    for (int i = 1; i <= max_iter; ++i) {
        double price = bs_call(S, K, r, T, sigma);
        double diff  = price - market_price;      // f(sigma)
        if (fabs(diff) < tol) return {sigma, i, true};

        double v = bs_vega(S, K, r, T, sigma);    // f'(sigma)
        if (v < 1e-12)                            // guard: flat slope
            return {sigma, i, false};

        sigma -= diff / v;                        // Newton step
        // keep sigma in sane bounds
        if (sigma < 1e-6) sigma = 1e-6;
        if (sigma > 5.0)  sigma = 5.0;
    }
    return {sigma, max_iter, false};              // did not converge
}

// Simulate a GBM random walk: dS/S = μ dt + σ dW
vector<double> simulate_gbm(int nSteps, double S0, double mu, double sigma, double T) {
    vector<double> path(nSteps+1);
    path[0] = S0;

    double dt = T / nSteps;
    mt19937 rng(42); // fixed seed for reproducibility
    normal_distribution<double> Z(0.0, 1.0);

    for (int i = 1; i <= nSteps; i++) {
        double dW = sqrt(dt) * Z(rng); // Brownian increment
        path[i] = path[i-1] * exp((mu - 0.5*sigma*sigma)*dt + sigma*dW);
    }
    return path;
}

// -------------------------
// Simple Matrix class
// -------------------------
class Matrix {
private:
    size_t rows_, cols_;
    vector<double> data_; // row-major

    size_t idx(size_t r, size_t c) const { return r * cols_ + c; }

public:
    Matrix(size_t r, size_t c, double val=0.0)
        : rows_(r), cols_(c), data_(r*c, val) {}

    double& operator()(size_t r, size_t c) { return data_[idx(r,c)]; }
    double operator()(size_t r, size_t c) const { return data_[idx(r,c)]; }

    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }

    void print(const string& name="") const {
        if (!name.empty()) cout << name << ":\n";
        for (size_t i = 0; i < rows_; i++) {
            for (size_t j = 0; j < cols_; j++) {
                cout << setw(10) << operator()(i,j) << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }

    // Matrix multiplication
    Matrix operator*(const Matrix& B) const {
        assert(cols_ == B.rows_);
        Matrix C(rows_, B.cols_);
        for (size_t i=0; i<rows_; i++) {
            for (size_t j=0; j<B.cols_; j++) {
                double sum=0;
                for (size_t k=0; k<cols_; k++) {
                    sum += (*this)(i,k) * B(k,j);
                }
                C(i,j)=sum;
            }
        }
        return C;
    }

    // Transpose
    Matrix transpose() const {
        Matrix T(cols_, rows_);
        for (size_t i=0; i<rows_; i++)
            for (size_t j=0; j<cols_; j++)
                T(j,i)=(*this)(i,j);
        return T;
    }
};

// -------------------------
// Covariance Matrix
// -------------------------
Matrix covariance(const Matrix& R) {
    size_t nObs = R.rows(), nAssets = R.cols();
    // compute mean
    vector<double> mean(nAssets,0.0);
    for (size_t j=0;j<nAssets;j++) {
        for (size_t i=0;i<nObs;i++) mean[j]+=R(i,j);
        mean[j]/=nObs;
    }
    // demean
    Matrix X(nObs,nAssets);
    for (size_t i=0;i<nObs;i++)
        for (size_t j=0;j<nAssets;j++)
            X(i,j)=R(i,j)-mean[j];

    // cov = (X^T * X)/(n-1)
    Matrix Xt = X.transpose();
    Matrix cov = Xt*X;
    for (size_t i=0;i<cov.rows();i++)
        for (size_t j=0;j<cov.cols();j++)
            cov(i,j)/=(nObs-1);
    return cov;
}

// -------------------------
// Generate random returns
// -------------------------
Matrix generate_returns(size_t nObs,size_t nAssets) {
    mt19937 rng(42);
    normal_distribution<double> norm(0.0,0.02);

    Matrix R(nObs,nAssets);
    for(size_t i=0;i<nObs;i++)
        for(size_t j=0;j<nAssets;j++)
            R(i,j)=norm(rng);
    return R;
}


int main() {
    cout << "=== C++ Quant Study Plan ===\n";
    cout << "[Day 8] STL Deep Dive\n\n";

    // 1. vector (contiguous array, cache-friendly)
    vector<int> v;
    for (int i = 0; i < 5; i++) v.push_back(i * 10);
    cout << "vector contents: ";
    for (auto x : v) cout << x << " ";
    cout << "\n";

    // 2. deque (fast push_front & push_back, but not contiguous)
    deque<int> dq;
    dq.push_back(1);
    dq.push_front(0);
    dq.push_back(2);
    cout << "deque contents: ";
    for (auto x : dq) cout << x << " ";
    cout << "\n";

    // 3. map (ordered, balanced BST, O(log n) ops)
    map<string, double> stockPrice;
    stockPrice["AAPL"] = 178.2;
    stockPrice["MSFT"] = 312.5;
    stockPrice["TSLA"] = 260.0;
    cout << "map (ordered): \n";
    for (auto& kv : stockPrice)
        cout << " " << kv.first << " : " << kv.second << "\n";

    // 4. unordered_map (hash table, avg O(1) ops, no ordering)
    unordered_map<string, double> fxRate;
    fxRate["USD/SGD"] = 1.36;
    fxRate["EUR/USD"] = 1.08;
    fxRate["BTC/USD"] = 68000.5;
    cout << "unordered_map (hash table): \n";
    for (auto& kv : fxRate)
        cout << " " << kv.first << " : " << kv.second << "\n";

    // --- Performance Demo ---
    const int N = 1000000;

    // vector push_back
    auto start = high_resolution_clock::now();
    vector<int> vec;
    for (int i = 0; i < N; i++) vec.push_back(i);
    auto end = high_resolution_clock::now();
    cout << "vector push_back " << N << " elements took "
         << duration_cast<milliseconds>(end - start).count() << " ms\n";

    // deque push_front
    start = high_resolution_clock::now();
    deque<int> deq;
    for (int i = 0; i < N; i++) deq.push_front(i);
    end = high_resolution_clock::now();
    cout << "deque push_front " << N << " elements took "
         << duration_cast<milliseconds>(end - start).count() << " ms\n";

    // map insert
    start = high_resolution_clock::now();
    map<int,int> mp;
    for (int i = 0; i < N; i++) mp[i] = i;
    end = high_resolution_clock::now();
    cout << "map insert " << N << " elements took "
         << duration_cast<milliseconds>(end - start).count() << " ms\n";

    // unordered_map insert
    start = high_resolution_clock::now();
    unordered_map<int,int> ump;
    for (int i = 0; i < N; i++) ump[i] = i;
    end = high_resolution_clock::now();
    cout << "unordered_map insert " << N << " elements took "
         << duration_cast<milliseconds>(end - start).count() << " ms\n";

    cout << "\n[Day 8] Done ✅\n";
    cout << "[Day 9] STL Algorithms with Lambdas\n\n";

    // Example dataset: returns
    vector<double> returns = {0.02, -0.01, 0.015, 0.03, -0.005, 0.025};

    // 1. sort
    sort(returns.begin(), returns.end());
    cout << "Sorted returns: ";
    for (auto r : returns) cout << r << " ";
    cout << "\n";

    // 2. nth_element (O(n) median/percentile finder)
    vector<double> returns2 = {0.02, -0.01, 0.015, 0.03, -0.005, 0.025};
    auto mid = returns2.begin() + returns2.size() / 2;
    nth_element(returns2.begin(), mid, returns2.end());
    cout << "Median return ~ " << *mid << "\n";

    // 3. accumulate (sum, mean, portfolio PnL)
    double sum = accumulate(returns.begin(), returns.end(), 0.0);
    double mean = sum / returns.size();
    cout << "Mean return = " << mean << "\n";

    // 4. transform (apply lambda to all elements)
    vector<double> scaled;
    scaled.resize(returns.size());
    transform(returns.begin(), returns.end(), scaled.begin(),
              [](double r) { return r * 100; }); // convert to %
    cout << "Returns in %: ";
    for (auto r : scaled) cout << r << "% ";
    cout << "\n";

    // Bonus: random shuffle + sort by absolute value
    random_device rd; mt19937 g(rd());
    shuffle(returns.begin(), returns.end(), g);
    sort(returns.begin(), returns.end(),
         [](double a, double b){ return fabs(a) < fabs(b); });
    cout << "Sorted by absolute value: ";
    for (auto r : returns) cout << r << " ";
    cout << "\n";

    cout << "\n[Day 9] Done ✅\n";

    cout << "[Day 10] Custom Allocators & Memory Pools\n\n";

    const int N = 1000000;

    // Default vector<int>
    auto start = high_resolution_clock::now();
    {
        vector<int> v;
        for (int i = 0; i < N; i++) v.push_back(i);
    }
    auto end = high_resolution_clock::now();
    cout << "Default vector allocation: "
         << duration_cast<milliseconds>(end - start).count() << " ms\n";

    // Custom allocator vector<int>
    start = high_resolution_clock::now();
    {
        MemoryPool pool(sizeof(int));
        vector<int, PoolAllocator<int>> v{PoolAllocator<int>(&pool)};
        for (int i = 0; i < N; i++) v.push_back(i);
    }
    end = high_resolution_clock::now();
    cout << "Vector with memory pool: "
         << duration_cast<milliseconds>(end - start).count() << " ms\n";

    cout << "\n[Day 10] Done ✅\n";

    cout << "[Day 11] Numerical Stability + Newton–Raphson\n\n";
    cout << fixed << setprecision(17);

    // ---- Floating-point quirks ----
    double a = 0.1 + 0.2;
    double b = 0.3;
    cout << "0.1 + 0.2 = " << a << "\n";
    cout << "0.3         " << b << "\n";
    cout << "Direct (==)? " << (a == b ? "true" : "false")
         << " | approx_equal? " << (approx_equal(a, b) ? "true" : "false") << "\n\n";

    // Associativity loss
    double big = 1e16, small = 1.0;
    double left  = (big + small) - big;   // loses 'small'
    double right = big + (small - big);   // reorders -> still problematic
    cout << "Associativity loss: (1e16 + 1) - 1e16 = " << left << "\n";
    cout << "                   big + (1 - big)   = " << right << "\n\n";

    // Kahan vs naive
    vector<double> tiny(1000000, 1e-10);
    double naive = accumulate(tiny.begin(), tiny.end(), 0.0);
    double kahan = kahan_sum(tiny);
    cout << "Naive sum of 1e6 * 1e-10  = " << setprecision(12) << naive << "\n";
    cout << "Kahan sum of 1e6 * 1e-10  = " << setprecision(12) << kahan << "\n\n";
    cout << setprecision(8);

    // long double shows more precision
    long double ld = 0.1L + 0.2L;
    cout << "long double (0.1 + 0.2) = " << setprecision(20) << (double)ld
         << "   [stored with higher precision internally]\n\n";
    cout << setprecision(8);

    // ---- Newton–Raphson: Implied Volatility ----
    double S = 100.0, K = 100.0, r = 0.05, T = 1.0;
    // Suppose market call price is 10.5; solve for sigma
    double market_price = 10.5;
    auto res = implied_vol_newton(market_price, S, K, r, T, /*init*/0.2);

    double check_price = bs_call(S, K, r, T, res.sigma);

    cout << "Market Call Price: " << market_price << "\n";
    cout << "Implied Vol (NR):  " << setprecision(10) << res.sigma
         << "  | iters: " << res.iters
         << "  | converged: " << (res.ok ? "yes" : "no") << "\n";
    cout << "BS price at IV:    " << setprecision(10) << check_price << "\n";

    cout << "\n[Day 11] Done ✅\n";

    cout << "[Day 12] Random Number Generators + Random Walk\n\n";

    // Basic RNG demo
    mt19937 rng(123); // fixed seed
    uniform_int_distribution<int> uni(1, 6);
    cout << "Rolling dice: ";
    for (int i = 0; i < 5; i++) cout << uni(rng) << " ";
    cout << "\n";

    normal_distribution<double> norm(0.0, 1.0);
    cout << "5 Normal(0,1) samples: ";
    for (int i = 0; i < 5; i++) cout << norm(rng) << " ";
    cout << "\n\n";

    // Simulate GBM path
    double S0 = 100.0, mu = 0.05, sigma = 0.2, T = 1.0;
    int nSteps = 10;
    auto path = simulate_gbm(nSteps, S0, mu, sigma, T);

    cout << "GBM Random Walk (S0=100, mu=5%, sigma=20%, T=1y):\n";
    cout << fixed << setprecision(4);
    for (int i = 0; i <= nSteps; i++) {
        cout << "Step " << i << ": " << path[i] << "\n";
    }

    cout << "\n[Day 12] Done ✅\n";
    cout<<"[Day 13] Matrix Ops (No Eigen) + Covariance\n\n";

    // Basic test
    Matrix A(2,2); A(0,0)=1;A(0,1)=2;A(1,0)=3;A(1,1)=4;
    Matrix B(2,2); B(0,0)=5;B(0,1)=6;B(1,0)=7;B(1,1)=8;

    A.print("Matrix A");
    B.print("Matrix B");
    Matrix C=A*B;
    C.print("A*B");

    // Covariance
    size_t nObs=1000,nAssets=3;
    Matrix R=generate_returns(nObs,nAssets);
    Matrix cov=covariance(R);
    cov.print("Covariance Matrix");

    cout<<"\n[Day 13] Done ✅\n";
    return 0;
}
