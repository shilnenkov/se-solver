#include "parameter_queue.h"

namespace SE {

ParameterQueue::ParameterQueue(uint32_t max_queue_size)
    : m_max_queue_size{max_queue_size}
{}

void ParameterQueue::push(const EquationParams& params)
{
    // If the queue is full, we need to wait until at least one consumer pops a parameter
    std::unique_lock lock{m_queue_mtx};
    m_not_full.wait(lock, [this] { return m_queue.size() < m_max_queue_size; });

    // Push new parameters to the queue and notify one consumer
    m_queue.push(params);
    lock.unlock();
    m_not_empty.notify_one();
}

void ParameterQueue::setDone()
{
    // Mark that there won't be new data
    m_done_flag.store(true);

    // Notify all threads
    m_not_full.notify_all();
    m_not_empty.notify_all();
}

bool ParameterQueue::pop(EquationParams* params)
{
    /*
     * Wait for new parameters to be enqueued or for a signal that producer finished populating the
     * queue
     */
    std::unique_lock lock{m_queue_mtx};
    m_not_empty.wait(lock, [this] { return !m_queue.empty() || m_done_flag; });

    /*
     * Notify consumer if there is no more data from the producer and all parameters have been
     * processed
     */
    if (m_done_flag.load() && m_queue.empty()) {
        return false;
    }

    // Pop params for processing
    *params = m_queue.front();
    m_queue.pop();

    // Notify producer that the queue isn't full now
    lock.unlock();
    m_not_full.notify_one();

    return true;
}

} // namespace SE
