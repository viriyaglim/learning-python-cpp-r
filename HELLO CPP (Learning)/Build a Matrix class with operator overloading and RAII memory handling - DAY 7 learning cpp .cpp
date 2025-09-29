#include <iostream>
#include <memory>
#include <cassert>
#include <iomanip>
#include <utility>

namespace day7 {

class RAIIMatrix {
private:
    size_t rows_{0}, cols_{0};
    std::unique_ptr<double[]> data_; // RAII-managed heap array (row-major)

    static size_t idx(size_t r, size_t c, size_t cols) { return r * cols + c; }

public:
    // --- ctors ---
    RAIIMatrix() = default;

    RAIIMatrix(size_t r, size_t c, double val = 0.0)
        : rows_(r), cols_(c), data_(std::make_unique<double[]>(r * c)) {
        for (size_t i = 0; i < r * c; ++i) data_[i] = val;
    }

    // --- copy ctor (deep copy) ---
    RAIIMatrix(const RAIIMatrix& other)
        : rows_(other.rows_), cols_(other.cols_),
          data_(other.data_ ? std::make_unique<double[]>(other.rows_ * other.cols_) : nullptr) {
        if (data_) {
            for (size_t i = 0; i < rows_ * cols_; ++i) data_[i] = other.data_[i];
        }
    }

    // --- copy assignment (copy-and-swap) ---
    RAIIMatrix& operator=(RAIIMatrix other) noexcept {
        std::swap(rows_, other.rows_);
        std::swap(cols_, other.cols_);
        std::swap(data_, other.data_);
        return *this;
    }

    // --- move ctor / move assignment ---
    RAIIMatrix(RAIIMatrix&& other) noexcept
        : rows_(other.rows_), cols_(other.cols_), data_(std::move(other.data_)) {
        other.rows_ = other.cols_ = 0;
    }

    RAIIMatrix& operator=(RAIIMatrix&& other) noexcept {
        if (this != &other) {
            rows_ = other.rows_;
            cols_ = other.cols_;
            data_ = std::move(other.data_);
            other.rows_ = other.cols_ = 0;
        }
        return *this;
    }

    // --- accessors ---
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }
    size_t size() const { return rows_ * cols_; }

    // --- element access ---
    double& operator()(size_t r, size_t c) {
        assert(r < rows_ && c < cols_);
        return data_[idx(r, c, cols_)];
    }
    double operator()(size_t r, size_t c) const {
        assert(r < rows_ && c < cols_);
        return data_[idx(r, c, cols_)];
    }

    // linear [] (row-major)
    double& operator[](size_t k) {
        assert(k < size());
        return data_[k];
    }
    double operator[](size_t k) const {
        assert(k < size());
        return data_[k];
    }

    // --- arithmetic ---
    RAIIMatrix operator+(const RAIIMatrix& b) const {
        assert(rows_ == b.rows_ && cols_ == b.cols_);
        RAIIMatrix out(rows_, cols_);
        for (size_t i = 0; i < size(); ++i) out.data_[i] = data_[i] + b.data_[i];
        return out;
    }

    RAIIMatrix operator-(const RAIIMatrix& b) const {
        assert(rows_ == b.rows_ && cols_ == b.cols_);
        RAIIMatrix out(rows_, cols_);
        for (size_t i = 0; i < size(); ++i) out.data_[i] = data_[i] - b.data_[i];
        return out;
    }

    // scalar multiply (right)
    RAIIMatrix operator*(double s) const {
        RAIIMatrix out(rows_, cols_);
        for (size_t i = 0; i < size(); ++i) out.data_[i] = data_[i] * s;
        return out;
    }

    // matrix multiply
    RAIIMatrix operator*(const RAIIMatrix& b) const {
        assert(cols_ == b.rows_);
        RAIIMatrix out(rows_, b.cols_, 0.0);
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t k = 0; k < cols_; ++k) {
                double aik = (*this)(i, k);
                for (size_t j = 0; j < b.cols_; ++j) {
                    out(i, j) += aik * b(k, j);
                }
            }
        }
        return out;
    }

    // --- utility ---
    static RAIIMatrix identity(size_t n) {
        RAIIMatrix I(n, n, 0.0);
        for (size_t i = 0; i < n; ++i) I(i, i) = 1.0;
        return I;
    }

    void print(const std::string& title = "") const {
        if (!title.empty()) std::cout << title << "\n";
        for (size_t r = 0; r < rows_; ++r) {
            for (size_t c = 0; c < cols_; ++c) {
                std::cout << std::setw(8) << operator()(r, c) << ' ';
            }
            std::cout << '\n';
        }
        std::cout << '\n';
    }

    // friend for scalar left-multiply: double * RAIIMatrix
    friend RAIIMatrix operator*(double s, const RAIIMatrix& A) { return A * s; }
};

} // namespace day7

int main() {
    using day7::RAIIMatrix;

    std::cout << "=== C++ Quant Study Plan ===\n";
    std::cout << "[Day 7] RAII Matrix with operator overloading\n\n";

    RAIIMatrix A(2, 3, 1.0);
    A(0, 1) = 5.0;
    A[4]    = 7.0;

    RAIIMatrix B(2, 3, 2.0);
    auto C = A + B;
    auto D = A - B;

    C.print("C = A + B:");
    D.print("D = A - B:");

    RAIIMatrix E(3, 2, 0.5);
    auto F = A * E;               // (2x3) * (3x2) = (2x2)
    F.print("F = A * E:");

    auto I = RAIIMatrix::identity(3);
    I.print("I3:");

    auto G = 2.0 * I;             // scalar left-multiply
    G.print("G = 2 * I3:");

    std::cout << "[Day 7] Done ✅\n";
    return 0;
}
