#include "consumer_pool.h"

namespace SE {

void ConsumerPool::run(const std::shared_ptr<ParameterQueue>& queue, unsigned consumer_count)
{
    // Reserve memory for the required consumer count
    m_consumers.reserve(consumer_count);

    // Create consumers and run them
    for (size_t i{0}; i < consumer_count; i++) {
        m_consumers.emplace_back(queue).run();
    }
}

void ConsumerPool::wait()
{
    // Wait for each consumer to stop and clear consumer list
    std::for_each(m_consumers.begin(), m_consumers.end(), [](auto& consumer) { consumer.wait(); });
    m_consumers.clear();
}

} // namespace SE
