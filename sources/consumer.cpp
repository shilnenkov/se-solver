#include "consumer.h"

#include <cmath>
#include <iostream>
#include <optional>
#include <sstream>

namespace {

/**
 * @brief Equation roots
 * @details It is suitable up to quadratic equation so there can be 0, 1 or 2 roots
 */
struct roots_t {
    std::optional<double> first_root{};
    std::optional<double> second_root{};
};

/**
 * @brief Check if this parameters belong to a linear equation
 * @details If the first parameter is zero, then 'f(x) = ax^2 + bx + c' will turn into
 * 'f(x) = bx + c', which is a linear equation
 * @param[in] params Equation parameters
 * @returns `true` if this is the linear equation
 * @returns `false` if this is a quadratic equation
 */
bool isLinearEquation(const SE::EquationParams &params)
{
    return params[0] == 0;
}

/**
 * @brief Solve a linear equation
 * @details Get root for 'f(x) = kx + b' equation
 * @param[in] k 'k' factor
 * @param[in] b 'b' factor
 * @returns Linear equation root if it is not an `std::nullopt` value
 * @returns `std::nullopt` if there is no root
 */
std::optional<double> solveLinearEquation(int64_t k, int64_t b)
{
    // If there is 'k' factor for 'f(x) = kx + b' equation, we can calculate the root as '-b / k'
    if (k != 0) {
        return static_cast<double>(-b) / k;
    }

    // There is no root
    return {};
}

/**
 * @brief Solve a quadratic equation
 * @details Get root for 'f(x) = ax^2 + bx + c' equation
 * @param[in] a 'a' factor
 * @param[in] b 'b' factor
 * @param[in] c 'c' factor
 * @returns Two roots if both `first_root` and `second_root` are not `std::nullopt`
 * @returns One root if `second_root` is `std::nullopt` and `first_root` is not
 * @returns No roots if both `first_root` and `second_root` are `std::nullopt`
 */
roots_t solveQuadraticEquation(int64_t a, int64_t b, int64_t c)
{
    // Calculating discriminant as 'd = b^2 - 4ac'
    int64_t discriminant = b * b - 4 * a * c;

    // If it's positive, there are 2 different roots '(-b +/- sqrt(discriminant)) / 2a'
    if (discriminant > 0) {
        double sqrt_d = std::sqrt(discriminant);
        return {
            (-b + sqrt_d) / (2.0 * a),
            (-b - sqrt_d) / (2.0 * a),
        };
    }

    // If it's zero, there is only one root, which can be obtained as '-b / 2a'
    if (discriminant == 0) {
        return {-b / (2.0 * a)};
    }

    // There are no real roots
    return {};
}

/**
 * @brief Solve equation for @p params parameters
 * @param[in] params Equation parameters
 * @returns Equation root(s) if exists
 */
roots_t solveEquation(const SE::EquationParams &params)
{
    // Solve a quadratic equation
    if (!isLinearEquation(params)) {
        return solveQuadraticEquation(params[0], params[1], params[2]);
    }

    // If the first parameter is zero, this is a linear equation
    return {solveLinearEquation(params[1], params[2])};
}

/**
 * @brief Output solved equation
 * @param[in] params Equation parameters
 * @param[in] roots Equation roots
 * @param[in] extremum Equation extremum
 */
void outputResult(const SE::EquationParams &params, const roots_t &roots, double extremum)
{
    // Output input parameters
    std::stringstream ss;
    ss << '(' << params[0] << ' ' << params[1] << ' ' << params[2] << ") => ";

    // Output roots
    if (roots.first_root.has_value() && roots.second_root.has_value()) {
        ss << '(' << roots.first_root.value() << ", " << roots.second_root.value() << "), ";
    } else if (roots.first_root.has_value()) {
        ss << '(' << roots.first_root.value() << "), ";
    } else {
        ss << "no roots, ";
    }

    /*
     * - If the first parameter is positive, we have Xmin extremum
     * - If the first parameter is negative, we have Xmax extremum
     * - If it is linear equation (the first parameter is zero), there is no extremum
     */
    if (params[0] > 0) {
        ss << "Xmin=" << extremum;
    } else if (params[0] < 0) {
        ss << "Xmax=" << extremum;
    } else {
        ss << "no extremum";
    }

    ss << std::endl;
    std::cout << ss.str();
}

} // namespace

namespace SE {

Consumer::Consumer(std::shared_ptr<ParameterQueue> queue)
    : m_queue{std::move(queue)}
{}

void Consumer::consumer()
{
    EquationParams params;

    // Pop parameters while the queue has data and there is no signal to stop processing input
    while (m_queue->pop(&params)) {
        auto roots = solveEquation(params);
        double extremum{};

        /*
         * Calculate extremum using the first derivative '2a*x + b = 0' if this is a quadratic
         * equation
         */
        if (!isLinearEquation(params)) {
            extremum = solveLinearEquation(2 * params[0], params[1]).value();
        }

        outputResult(params, roots, extremum);
    }
}

void Consumer::run()
{
    // Run consumer thread
    m_consumer_thread = std::thread{&Consumer::consumer, this};
}

void Consumer::wait()
{
    // Join consumer thread if it was run
    if (m_consumer_thread.joinable()) {
        m_consumer_thread.join();
    }
}

} // namespace SE
