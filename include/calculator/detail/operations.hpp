#pragma once

#include <cmath>
#include <type_traits>

#include "calculator/exceptions.hpp"

/**
 * @file detail/operations.hpp
 * @brief Low-level arithmetic primitives used internally by Calculator.
 *
 * @note This is an implementation-detail header. Prefer including
 *       `calculator/calculator.hpp` in user code.
 */

namespace calculator {
namespace detail {

/**
 * @brief Returns the sum of two values.
 * @tparam T Arithmetic type.
 * @param  a Left operand.
 * @param  b Right operand.
 * @return   @p a + @p b
 */
template <typename T>
constexpr T add(T a, T b) noexcept {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    return a + b;
}

/**
 * @brief Returns the difference of two values.
 * @tparam T Arithmetic type.
 * @param  a Minuend.
 * @param  b Subtrahend.
 * @return   @p a - @p b
 */
template <typename T>
constexpr T subtract(T a, T b) noexcept {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    return a - b;
}

/**
 * @brief Returns the product of two values.
 * @tparam T Arithmetic type.
 * @param  a Left factor.
 * @param  b Right factor.
 * @return   @p a * @p b
 */
template <typename T>
constexpr T multiply(T a, T b) noexcept {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    return a * b;
}

/**
 * @brief Returns the quotient of two values.
 * @tparam T Arithmetic type.
 * @param  a Dividend.
 * @param  b Divisor.
 * @return   @p a / @p b
 * @throws DivisionByZeroException if @p b is zero.
 */
template <typename T>
T divide(T a, T b) {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    if (b == T(0)) {
        throw calculator::DivisionByZeroException();
    }
    return a / b;
}

/**
 * @brief Raises @p base to an integer power using binary exponentiation (O(log n)).
 *
 * Negative exponents: floating-point types return `1/base^|exp|`;
 * integer types return `0` (consistent with truncated integer semantics).
 *
 * @tparam T   Arithmetic type.
 * @param  base The base value.
 * @param  exp  The exponent (may be negative).
 * @return      @p base raised to @p exp.
 */
template <typename T>
T power(T base, int exp) noexcept {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    if (exp == 0) return T(1);

    bool         negative = (exp < 0);
    unsigned int uexp     = negative ? static_cast<unsigned int>(-exp)
                                     : static_cast<unsigned int>(exp);
    T result(1);
    while (uexp > 0u) {
        if (uexp & 1u) result *= base;
        base  *= base;
        uexp >>= 1u;
    }
    // For floating-point: T(1)/result gives 1/base^|exp|.
    // For integers:       T(1)/result is 0 for any base > 1 — same as T(0).
    return negative ? T(1) / result : result;
}

/**
 * @brief Computes the non-negative square root of @p value.
 * @tparam T  Floating-point type.
 * @param  value Non-negative radicand.
 * @return       Square root of @p value.
 * @throws InvalidArgumentException if @p value is negative.
 */
template <typename T>
T sqrt(T value) {
    static_assert(std::is_floating_point<T>::value,
                  "sqrt requires a floating-point type");
    if (value < T(0)) {
        throw calculator::InvalidArgumentException("sqrt",
                                                   static_cast<double>(value));
    }
    return std::sqrt(value);
}

/**
 * @brief Computes the absolute value of @p value.
 * @tparam T Arithmetic type.
 * @param  value Input value.
 * @return       |value|
 */
template <typename T>
constexpr T abs(T value) noexcept {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    return value < T(0) ? -value : value;
}

/**
 * @brief Returns the larger of two values.
 * @tparam T Arithmetic type.
 */
template <typename T>
constexpr T max(T a, T b) noexcept {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    return a > b ? a : b;
}

/**
 * @brief Returns the smaller of two values.
 * @tparam T Arithmetic type.
 */
template <typename T>
constexpr T min(T a, T b) noexcept {
    static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
    return a < b ? a : b;
}

} // namespace detail
} // namespace calculator
