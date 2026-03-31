/**
 * @file test_calculator.cpp
 * @brief Unit tests for the Calculator header-only library.
 *
 * Test suites
 * ───────────
 * - CalculatorDoubleTest  — double-precision arithmetic
 * - CalculatorIntTest     — integer arithmetic
 * - CalculatorExceptions  — exception contracts
 * - CalculatorHistory     — history and state
 * - CalculatorAliases     — convenience type aliases
 * - DetailOperations      — internal free-function primitives
 */

#include <cmath>
#include <limits>

#include <gtest/gtest.h>

#include "calculator/calculator.hpp"
#include "calculator/detail/operations.hpp"
#include "calculator/exceptions.hpp"

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────
namespace {
constexpr double kEps = 1e-9;
} // namespace

// ═════════════════════════════════════════════════════════════════════════════
// Suite 1 — Double-precision calculator
// ═════════════════════════════════════════════════════════════════════════════
class CalculatorDoubleTest : public ::testing::Test {
protected:
    calculator::DoubleCalculator calc;
};

TEST_F(CalculatorDoubleTest, AddPositives) {
    EXPECT_NEAR(calc.add(2.0, 3.0), 5.0, kEps);
}

TEST_F(CalculatorDoubleTest, AddNegatives) {
    EXPECT_NEAR(calc.add(-2.0, -3.0), -5.0, kEps);
}

TEST_F(CalculatorDoubleTest, AddMixed) {
    EXPECT_NEAR(calc.add(-1.5, 1.5), 0.0, kEps);
}

TEST_F(CalculatorDoubleTest, SubtractBasic) {
    EXPECT_NEAR(calc.subtract(10.0, 4.0), 6.0, kEps);
}

TEST_F(CalculatorDoubleTest, SubtractYieldsNegative) {
    EXPECT_NEAR(calc.subtract(3.0, 5.0), -2.0, kEps);
}

TEST_F(CalculatorDoubleTest, MultiplyBasic) {
    EXPECT_NEAR(calc.multiply(3.0, 4.0), 12.0, kEps);
}

TEST_F(CalculatorDoubleTest, MultiplyByZero) {
    EXPECT_NEAR(calc.multiply(999.0, 0.0), 0.0, kEps);
}

TEST_F(CalculatorDoubleTest, MultiplyNegatives) {
    EXPECT_NEAR(calc.multiply(-3.0, -4.0), 12.0, kEps);
}

TEST_F(CalculatorDoubleTest, DivideBasic) {
    EXPECT_NEAR(calc.divide(10.0, 4.0), 2.5, kEps);
}

TEST_F(CalculatorDoubleTest, DivideNegative) {
    EXPECT_NEAR(calc.divide(-9.0, 3.0), -3.0, kEps);
}

TEST_F(CalculatorDoubleTest, PowerPositiveExponent) {
    EXPECT_NEAR(calc.power(2.0, 10), 1024.0, kEps);
}

TEST_F(CalculatorDoubleTest, PowerZeroExponent) {
    EXPECT_NEAR(calc.power(42.0, 0), 1.0, kEps);
}

TEST_F(CalculatorDoubleTest, PowerNegativeExponent) {
    EXPECT_NEAR(calc.power(2.0, -3), 0.125, kEps);
}

TEST_F(CalculatorDoubleTest, SqrtPerfectSquare) {
    EXPECT_NEAR(calc.sqrt(16.0), 4.0, kEps);
}

TEST_F(CalculatorDoubleTest, SqrtZero) {
    EXPECT_NEAR(calc.sqrt(0.0), 0.0, kEps);
}

TEST_F(CalculatorDoubleTest, SqrtIrrational) {
    EXPECT_NEAR(calc.sqrt(2.0), std::sqrt(2.0), kEps);
}

TEST_F(CalculatorDoubleTest, AbsPositive) {
    EXPECT_NEAR(calc.abs(7.5), 7.5, kEps);
}

TEST_F(CalculatorDoubleTest, AbsNegative) {
    EXPECT_NEAR(calc.abs(-7.5), 7.5, kEps);
}

TEST_F(CalculatorDoubleTest, AbsZero) {
    EXPECT_NEAR(calc.abs(0.0), 0.0, kEps);
}

TEST_F(CalculatorDoubleTest, MaxReturnsLarger) {
    EXPECT_NEAR(calc.max(3.0, 7.0), 7.0, kEps);
}

TEST_F(CalculatorDoubleTest, MaxEqualValues) {
    EXPECT_NEAR(calc.max(5.0, 5.0), 5.0, kEps);
}

TEST_F(CalculatorDoubleTest, MinReturnsSmaller) {
    EXPECT_NEAR(calc.min(3.0, 7.0), 3.0, kEps);
}

// ═════════════════════════════════════════════════════════════════════════════
// Suite 2 — Integer calculator
// ═════════════════════════════════════════════════════════════════════════════
class CalculatorIntTest : public ::testing::Test {
protected:
    calculator::IntCalculator calc;
};

TEST_F(CalculatorIntTest, AddBasic) {
    EXPECT_EQ(calc.add(2, 3), 5);
}

TEST_F(CalculatorIntTest, SubtractBasic) {
    EXPECT_EQ(calc.subtract(10, 4), 6);
}

TEST_F(CalculatorIntTest, MultiplyBasic) {
    EXPECT_EQ(calc.multiply(6, 7), 42);
}

TEST_F(CalculatorIntTest, DivideIntegerTruncates) {
    EXPECT_EQ(calc.divide(7, 2), 3);  // integer truncation
}

TEST_F(CalculatorIntTest, PowerBasic) {
    EXPECT_EQ(calc.power(3, 4), 81);
}

TEST_F(CalculatorIntTest, PowerNegativeExponentIntegerIsZero) {
    // integer ^ -n → 0 by integer semantics
    EXPECT_EQ(calc.power(3, -2), 0);
}

TEST_F(CalculatorIntTest, AbsNegative) {
    EXPECT_EQ(calc.abs(-42), 42);
}

TEST_F(CalculatorIntTest, MaxBothNegative) {
    EXPECT_EQ(calc.max(-3, -1), -1);
}

TEST_F(CalculatorIntTest, MinBothNegative) {
    EXPECT_EQ(calc.min(-3, -1), -3);
}

// ═════════════════════════════════════════════════════════════════════════════
// Suite 3 — Exception contracts
// ═════════════════════════════════════════════════════════════════════════════
class CalculatorExceptions : public ::testing::Test {
protected:
    calculator::DoubleCalculator dcalc;
    calculator::IntCalculator    icalc;
};

TEST_F(CalculatorExceptions, DivideByZeroDoubleThrows) {
    EXPECT_THROW(dcalc.divide(1.0, 0.0),
                 calculator::DivisionByZeroException);
}

TEST_F(CalculatorExceptions, DivideByZeroIntThrows) {
    EXPECT_THROW(icalc.divide(5, 0),
                 calculator::DivisionByZeroException);
}

TEST_F(CalculatorExceptions, DivideByZeroIsCalculatorException) {
    // DivisionByZeroException must be catchable as the base type
    EXPECT_THROW(dcalc.divide(1.0, 0.0),
                 calculator::CalculatorException);
}

TEST_F(CalculatorExceptions, DivideByZeroIsRuntimeError) {
    EXPECT_THROW(dcalc.divide(1.0, 0.0), std::runtime_error);
}

TEST_F(CalculatorExceptions, SqrtNegativeThrows) {
    EXPECT_THROW(dcalc.sqrt(-1.0),
                 calculator::InvalidArgumentException);
}

TEST_F(CalculatorExceptions, SqrtNegativeIsCalculatorException) {
    EXPECT_THROW(dcalc.sqrt(-0.001),
                 calculator::CalculatorException);
}

TEST_F(CalculatorExceptions, DivisionByZeroMessageNotEmpty) {
    try {
        icalc.divide(1, 0);
        FAIL() << "Expected DivisionByZeroException";
    } catch (const calculator::DivisionByZeroException& e) {
        EXPECT_FALSE(std::string(e.what()).empty());
    }
}

TEST_F(CalculatorExceptions, InvalidArgumentMessageContainsOperation) {
    try {
        dcalc.sqrt(-4.0);
        FAIL() << "Expected InvalidArgumentException";
    } catch (const calculator::InvalidArgumentException& e) {
        EXPECT_NE(std::string(e.what()).find("sqrt"), std::string::npos);
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// Suite 4 — History and state
// ═════════════════════════════════════════════════════════════════════════════
class CalculatorHistory : public ::testing::Test {
protected:
    calculator::DoubleCalculator calc;       // history ON (default)
    calculator::DoubleCalculator no_hist{false}; // history OFF
};

TEST_F(CalculatorHistory, StartsEmpty) {
    EXPECT_TRUE(calc.history().empty());
    EXPECT_EQ(calc.operation_count(), 0u);
}

TEST_F(CalculatorHistory, LastResultEmptyOnStart) {
    EXPECT_EQ(calc.last_result(), static_cast<const double*>(0));
}

TEST_F(CalculatorHistory, RecordsResults) {
    calc.add(1.0, 2.0);
    calc.multiply(3.0, 4.0);
    ASSERT_EQ(calc.history().size(), 2u);
    EXPECT_NEAR(calc.history()[0], 3.0, kEps);
    EXPECT_NEAR(calc.history()[1], 12.0, kEps);
}

TEST_F(CalculatorHistory, LastResultReturnsLatest) {
    calc.add(5.0, 5.0);
    calc.subtract(3.0, 1.0);
    ASSERT_NE(calc.last_result(), static_cast<const double*>(0));
    EXPECT_NEAR(*calc.last_result(), 2.0, kEps);
}

TEST_F(CalculatorHistory, ClearHistoryEmptiesBuffer) {
    calc.add(1.0, 1.0);
    calc.clear_history();
    EXPECT_TRUE(calc.history().empty());
    EXPECT_EQ(calc.last_result(), static_cast<const double*>(0));
}

TEST_F(CalculatorHistory, OperationCountIncrementsCorrectly) {
    for (int i = 0; i < 5; ++i) calc.add(double(i), 0.0);
    EXPECT_EQ(calc.operation_count(), 5u);
}

TEST_F(CalculatorHistory, NoHistoryModeDoesNotRecord) {
    no_hist.add(1.0, 2.0);
    no_hist.multiply(3.0, 4.0);
    EXPECT_TRUE(no_hist.history().empty());
    EXPECT_EQ(no_hist.operation_count(), 0u);
    EXPECT_EQ(no_hist.last_result(), static_cast<const double*>(0));
}

TEST_F(CalculatorHistory, ExceptionDoesNotPollute) {
    calc.add(1.0, 1.0);
    EXPECT_THROW(calc.divide(1.0, 0.0), calculator::DivisionByZeroException);
    // History must remain unchanged after a thrown exception
    EXPECT_EQ(calc.operation_count(), 1u);
}

// ═════════════════════════════════════════════════════════════════════════════
// Suite 5 — Convenience aliases
// ═════════════════════════════════════════════════════════════════════════════
TEST(CalculatorAliases, DoubleCalculatorWorks) {
    calculator::DoubleCalculator c;
    EXPECT_NEAR(c.add(1.1, 2.2), 3.3, 1e-6);
}

TEST(CalculatorAliases, FloatCalculatorWorks) {
    calculator::FloatCalculator c;
    EXPECT_NEAR(c.multiply(2.f, 3.f), 6.f, 1e-5f);
}

TEST(CalculatorAliases, IntCalculatorWorks) {
    calculator::IntCalculator c;
    EXPECT_EQ(c.subtract(10, 3), 7);
}

TEST(CalculatorAliases, LongCalculatorWorks) {
    calculator::LongCalculator c;
    EXPECT_EQ(c.power(2LL, 32), 4294967296LL);
}

// ═════════════════════════════════════════════════════════════════════════════
// Suite 6 — detail:: free-function primitives
// ═════════════════════════════════════════════════════════════════════════════
TEST(DetailOperations, AddIsConstexpr) {
    constexpr int r = calculator::detail::add(3, 4);
    static_assert(r == 7, "constexpr add must evaluate at compile time");
    EXPECT_EQ(r, 7);
}

TEST(DetailOperations, SubtractIsConstexpr) {
    constexpr double r = calculator::detail::subtract(10.0, 4.5);
    EXPECT_NEAR(r, 5.5, kEps);
}

TEST(DetailOperations, MultiplyIsConstexpr) {
    constexpr int r = calculator::detail::multiply(6, 7);
    static_assert(r == 42, "constexpr multiply");
    EXPECT_EQ(r, 42);
}

TEST(DetailOperations, DivideIntThrowsOnZero) {
    EXPECT_THROW(calculator::detail::divide(1, 0),
                 calculator::DivisionByZeroException);
}

TEST(DetailOperations, PowerComputesCorrectly) {
    // power() uses a runtime loop in C++11 (constexpr loops require C++14+)
    EXPECT_EQ(calculator::detail::power(2, 8), 256);
}

TEST(DetailOperations, SqrtNegativeThrows) {
    EXPECT_THROW(calculator::detail::sqrt(-1.0),
                 calculator::InvalidArgumentException);
}

TEST(DetailOperations, AbsIsConstexpr) {
    constexpr int r = calculator::detail::abs(-99);
    static_assert(r == 99, "constexpr abs");
    EXPECT_EQ(r, 99);
}

TEST(DetailOperations, MaxIsConstexpr) {
    constexpr int r = calculator::detail::max(3, 7);
    static_assert(r == 7, "constexpr max");
    EXPECT_EQ(r, 7);
}

TEST(DetailOperations, MinIsConstexpr) {
    constexpr int r = calculator::detail::min(3, 7);
    static_assert(r == 3, "constexpr min");
    EXPECT_EQ(r, 3);
}
