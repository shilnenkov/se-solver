#pragma once

#include "parameter_queue.h"

#include <memory>
#include <thread>

namespace SE {

/**
 * @brief Equation parameters consumer
 */
class Consumer
{
public:
    /**
     * @brief Constructor
     * @param[in] queue Equation parameters queue
     */
    explicit Consumer(std::shared_ptr<ParameterQueue> queue);

    /**
     * @brief Run the consumer in a separate thread
     */
    void run();

    /**
     * @brief Wait until the consumer processes all parameters from the queue
     */
    void wait();

private:
    /**
     * @brief Consumer thread
     * @details It fetches parameters from the queue, solves equations and outputs result
     */
    void consumer();

    std::shared_ptr<ParameterQueue> m_queue; ///< Equation parameters queue
    std::thread m_consumer_thread;           ///< Consumer thread
};

} // namespace SE
