#pragma once

#include "parameter_queue.h"

#include <thread>

namespace SE {

/**
 * @brief Equation parameters producer
 * @details It converts input arguments to the parameters and pushes them to the queue
 */
class Producer
{
public:
    /**
     * @brief Constructor
     * @param[in] queue Equation parameter queue
     */
    explicit Producer(std::shared_ptr<ParameterQueue> queue);

    /**
     * @brief Run the producer in a separate thread
     * @param[in] parameters List of parameters as string array
     * @param[in] parameter_count The number of parameters
     */
    void run(char** parameters, int parameter_count);

    /**
     * Wait until the producer finished population the queue
     */
    void wait();

private:
    /**
     * @brief Producer thread
     * @details It convert @p parameters list to set of @ref EquationParams and push them to
     * @p ParameterQueue
     * @param[in] parameters List of parameters as string array
     * @param[in] parameter_count The number of parameters
     */
    void producer(char** parameters, int parameter_count);

    std::shared_ptr<ParameterQueue> m_queue; ///< Equation parameters queue
    std::thread m_producer_thread;           ///< Producer thread
};

} // namespace SE
