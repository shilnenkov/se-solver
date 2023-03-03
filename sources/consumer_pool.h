#pragma once

#include "consumer.h"
#include "parameter_queue.h"

#include <vector>

namespace SE {

/**
 * @brief A consumer pool which helps to manage list of concurrent consumers
 */
class ConsumerPool
{
public:
    /**
     * @brief Run @p consumer_count consumers concurrently
     * @param[in] queue Equation parameter queue
     * @param[in] consumer_count Required consumer count
     */
    void run(const std::shared_ptr<ParameterQueue>& queue, unsigned consumer_count);

    /**
     * @brief Wait until all consumers finished solving equations
     */
    void wait();

private:
    std::vector<Consumer> m_consumers; ///< Consumer list
};

} // namespace SE
