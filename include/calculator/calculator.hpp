#pragma once

/**
 * @file calculator.hpp
 * @brief Primary public header for the Calculator header-only library.
 *
 * @mainpage Calculator Library
 *
 * @section intro_sec Introduction
 * A modern, header-only C++11 arithmetic library providing a type-safe
 * calculator template with full exception support and Doxygen/Graphviz docs.
 *
 * @section usage_sec Quick Start
 * @code{.cpp}
 * #include <calculator/calculator.hpp>
 *
 * calculator::Calculator<double> calc;
 * double result = calc.add(3.0, 4.0);          // 7.0
 * double sq     = calc.sqrt(16.0);              // 4.0
 * double pw     = calc.power(2.0, 10);          // 1024.0
 * @endcode
 *
 * @section install_sec CMake Integration
 * @code{.cmake}
 * find_package(Calculator CONFIG REQUIRED)
 * target_link_libraries(my_target PRIVATE Calculator::calculator)
 * @endcode
 *
 * @version 1.0.0
 * @author  Calculator Library Contributors
 * @copyright MIT License
 */

#include <cstddef>
#include <string>
#include <type_traits>
#include <vector>

#include "calculator/detail/operations.hpp"
#include "calculator/exceptions.hpp"

namespace calculator {

/**
 * @brief A stateful, type-safe arithmetic calculator.
 *
 * `Calculator<T>` wraps the low-level primitives in `calculator::detail`
 * and optionally records a history of results produced during its lifetime.
 *
 * @tparam T Arithmetic type used for all computations (e.g. `double`, `int`).
 *
 * @par Constraints
 * `T` must satisfy `std::is_arithmetic<T>::value`.
 *
 * @par Thread Safety
 * A single `Calculator` instance is **not** thread-safe.
 * Use separate instances per thread or synchronise externally.
 *
 * @par Example
 * @code{.cpp}
 * calculator::Calculator<double> calc;
 * double r = calc.add(1.5, 2.5);       // r == 4.0
 * double s = calc.divide(10.0, 4.0);   // s == 2.5
 * auto&  h = calc.history();           // {4.0, 2.5}
 * @endcode
 */
template <typename T>
class Calculator {
    static_assert(std::is_arithmetic<T>::value,
                  "Calculator<T>: T must be an arithmetic type");

public:
    // ------------------------------------------------------------------ //
    //  Types
    // ------------------------------------------------------------------ //

    /// Underlying value type.
    typedef T value_type;

    // ------------------------------------------------------------------ //
    //  Construction
    // ------------------------------------------------------------------ //

    /**
     * @brief Default constructor.
     * @param record_history When `true` every result is appended to the
     *        internal history buffer; when `false` history is disabled.
     */
    explicit Calculator(bool record_history = true) noexcept
        : record_history_(record_history) {}

    Calculator(const Calculator&) = default;
    Calculator(Calculator&&) noexcept = default;
    Calculator& operator=(const Calculator&) = default;
    Calculator& operator=(Calculator&&) noexcept = default;
    ~Calculator() = default;

    // ------------------------------------------------------------------ //
    //  Basic arithmetic
    // ------------------------------------------------------------------ //

    /**
     * @brief Adds two values.
     * @param a Left operand.
     * @param b Right operand.
     * @return  `a + b`
     */
    T add(T a, T b) noexcept {
        return record(detail::add(a, b));
    }

    /**
     * @brief Subtracts @p b from @p a.
     * @param a Minuend.
     * @param b Subtrahend.
     * @return  `a - b`
     */
    T subtract(T a, T b) noexcept {
        return record(detail::subtract(a, b));
    }

    /**
     * @brief Multiplies two values.
     * @param a Left factor.
     * @param b Right factor.
     * @return  `a * b`
     */
    T multiply(T a, T b) noexcept {
        return record(detail::multiply(a, b));
    }

    /**
     * @brief Divides @p a by @p b.
     * @param a Dividend.
     * @param b Divisor — must not be zero.
     * @return  `a / b`
     * @throws  DivisionByZeroException if `b == 0`.
     */
    T divide(T a, T b) {
        return record(detail::divide(a, b));
    }

    // ------------------------------------------------------------------ //
    //  Advanced operations
    // ------------------------------------------------------------------ //

    /**
     * @brief Raises @p base to the @p exp -th power.
     *
     * Uses binary exponentiation (O(log exp)).
     *
     * @param base Base value.
     * @param exp  Integer exponent (negative exponents supported for
     *             floating-point `T`; integer `T` returns 0 for negative
     *             exponents).
     * @return     `base ^ exp`
     */
    T power(T base, int exp) noexcept {
        return record(detail::power(base, exp));
    }

    /**
     * @brief Computes the square root.
     *
     * Only available when `T` is a floating-point type.
     *
     * @param value Non-negative radicand.
     * @return      √value
     * @throws      InvalidArgumentException if `value < 0`.
     */
    T sqrt(T value) {
        static_assert(std::is_floating_point<T>::value,
                      "sqrt() requires a floating-point Calculator");
        return record(detail::sqrt(value));
    }

    /**
     * @brief Returns the absolute value.
     * @param value Input value.
     * @return      |value|
     */
    T abs(T value) noexcept {
        return record(detail::abs(value));
    }

    /**
     * @brief Returns the larger of two values.
     * @param a First value.
     * @param b Second value.
     * @return  max(a, b)
     */
    T max(T a, T b) noexcept {
        return record(detail::max(a, b));
    }

    /**
     * @brief Returns the smaller of two values.
     * @param a First value.
     * @param b Second value.
     * @return  min(a, b)
     */
    T min(T a, T b) noexcept {
        return record(detail::min(a, b));
    }

    // ------------------------------------------------------------------ //
    //  History
    // ------------------------------------------------------------------ //

    /**
     * @brief Read-only view of the result history.
     *
     * Contains every value returned by arithmetic methods in the order
     * they were computed (only when history was enabled at construction).
     *
     * @return Const reference to the history vector.
     */
    const std::vector<T>& history() const noexcept {
        return history_;
    }

    /**
     * @brief Returns a pointer to the most recent result, or null if none.
     *
     * The pointer is invalidated by any subsequent arithmetic call (since
     * the underlying vector may reallocate).
     *
     * @return Pointer to the last element, or `nullptr` if history is empty.
     */
    const T* last_result() const noexcept {
        if (history_.empty()) return static_cast<const T*>(0);
        return &history_.back();
    }

    /**
     * @brief Clears the history buffer.
     */
    void clear_history() noexcept { history_.clear(); }

    /**
     * @brief Returns the number of operations recorded in history.
     * @return History size.
     */
    std::size_t operation_count() const noexcept {
        return history_.size();
    }

private:
    bool           record_history_;
    std::vector<T> history_;

    /**
     * @brief Appends @p value to the history buffer (if enabled) and returns it.
     * @param value Result to record.
     * @return      @p value unchanged.
     */
    T record(T value) {
        if (record_history_) history_.push_back(value);
        return value;
    }
};

// ------------------------------------------------------------------ //
//  Convenience type aliases
// ------------------------------------------------------------------ //

/// @brief Double-precision floating-point calculator.
typedef Calculator<double>    DoubleCalculator;

/// @brief Single-precision floating-point calculator.
typedef Calculator<float>     FloatCalculator;

/// @brief Integer calculator.
typedef Calculator<int>       IntCalculator;

/// @brief Long integer calculator.
typedef Calculator<long long> LongCalculator;

} // namespace calculator
