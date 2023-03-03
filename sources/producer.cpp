#include "producer.h"

#include <iostream>
#include <sstream>

namespace {

/**
 * @brief Output an error message for specific parameters
 * @details This function outputs @p count parameters with @p offset and appends a custom message
 * @param[in] parameters Parameters list
 * @param[in] offset Offset of the current parameters
 * @param[in] count The number of parameters to be displayed
 * @param[in] message A custom message
 */
void outputError(char** parameters, int offset, int count, std::string_view message)
{
    std::stringstream ss;

    // Add the first parameter
    ss << '(' << parameters[offset];

    // Output the rest of parameters separated by spaces
    for (int i{1}; i < count; i++) {
        ss << ' ' << parameters[offset + i];
    }

    // Output message
    ss << ") => " << message << std::endl;
    std::cerr << ss.str();
}

/**
 * @brief Convert a string to a number
 * @param[in] string String containg an integer value
 * @param[out] number Integer number
 * @returns `true` on success
 * @returns `false` on failure
 */
bool toNumber(std::string_view string, int64_t* number)
{
    size_t final_pos{0};

    try {
        *number = std::stoll(string.data(), &final_pos);
    } catch (const std::invalid_argument& e) {
        std::cerr << '\'' << string << "' can't be converted to an integer value" << std::endl;
        return false;
    } catch (const std::out_of_range& e) {
        std::cerr << '\'' << string << "' value is out of range" << std::endl;
        return false;
    }

    if (string.size() != final_pos) {
        std::cerr << '\'' << string << "' can't be fully converted to an integer" << std::endl;
        return false;
    }

    return true;
}

/**
 * @brief Parse a list of string parameters into square equation parameters
 * @param[in] parameters List of equation parameters
 * @param[in] offset Parameters offset
 * @param[out] params Parsed parameters
 * @returns `true` on success
 * @returns `false` on failure
 */
bool parseParameters(char** parameters, int offset, SE::EquationParams* params)
{
    for (size_t i{0}; i < params->size(); i++) {
        if (!toNumber(parameters[offset + i], &params->at(i))) {
            outputError(parameters, offset, params->size(),
                        "failed to convert parameters to integers");
            return false;
        }
    }

    return true;
}

} // namespace

namespace SE {

Producer::Producer(std::shared_ptr<ParameterQueue> queue)
    : m_queue{std::move(queue)}
{}

void Producer::run(char** parameters, int parameter_count)
{
    // Run producer thread with given parameters
    m_producer_thread = std::thread{&Producer::producer, this, parameters, parameter_count};
}

void Producer::wait()
{
    // Join producer thread if it has been run
    if (m_producer_thread.joinable()) {
        m_producer_thread.join();
    }
}

void Producer::producer(char** parameters, int parameter_count)
{
    EquationParams params;

    // Declare offset outside the for loop to be able to check if all parameters have been processed
    int offset{0};

    /*
     * There are `EquationParams::size()` number of parameters for each equation, so we convert each
     * parameter set from strings to integer values and push them to the queue
     */
    for (; offset + params.size() <= parameter_count; offset += params.size()) {
        if (parseParameters(parameters, offset, &params)) {
            m_queue->push(params);
        }
    }

    // Mark that the producer finished its work and there won't be new parameters
    m_queue->setDone();

    // Output error message if there are unprocessed parameters
    if (offset < parameter_count) {
        outputError(parameters, offset, parameter_count - offset,
                    "Unprocessed parameters: parameter count must be a multiple of 3!");
    }
}

} // namespace SE
