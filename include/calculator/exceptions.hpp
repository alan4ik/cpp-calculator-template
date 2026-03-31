#pragma once

#include <stdexcept>
#include <string>

/**
 * @file exceptions.hpp
 * @brief Custom exception hierarchy for the Calculator library.
 *
 * @author Calculator Library
 * @version 1.0.0
 */

namespace calculator {

/**
 * @brief Base exception class for all calculator errors.
 *
 * All exceptions thrown by the Calculator library derive from this class,
 * allowing callers to catch every calculator error with a single handler.
 */
class CalculatorException : public std::runtime_error {
public:
    /**
     * @brief Constructs a CalculatorException with the given message.
     * @param message Human-readable description of the error.
     */
    explicit CalculatorException(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @brief Exception thrown when a division-by-zero is attempted.
 *
 * @par Example
 * @code
 * try {
 *     calc.divide(10.0, 0.0);
 * } catch (const calculator::DivisionByZeroException& e) {
 *     std::cerr << e.what() << '\n';
 * }
 * @endcode
 */
class DivisionByZeroException : public CalculatorException {
public:
    DivisionByZeroException()
        : CalculatorException("Division by zero is undefined") {}
};

/**
 * @brief Exception thrown when an argument is mathematically invalid.
 *
 * Raised for operations such as square root of a negative number.
 *
 * @par Example
 * @code
 * try {
 *     calc.sqrt(-4.0);
 * } catch (const calculator::InvalidArgumentException& e) {
 *     std::cerr << e.what() << '\n';
 * }
 * @endcode
 */
class InvalidArgumentException : public CalculatorException {
public:
    /**
     * @brief Constructs the exception with an operation name and offending value.
     * @param operation Name of the operation that failed (e.g. "sqrt").
     * @param value     The invalid argument value.
     */
    explicit InvalidArgumentException(const std::string& operation,
                                      double value)
        : CalculatorException("Invalid argument for '" + operation +
                              "': " + std::to_string(value)) {}
};

} // namespace calculator
