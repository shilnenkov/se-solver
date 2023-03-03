#pragma once

#include <array>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace SE {

/// Three equation parameters [a, b, c] for 'f(x) = ax^2 + bx + c' quadratic equation
using EquationParams = std::array<int64_t, 3>;

/**
 * @brief Blocking queue to deliver equation parameters from producer to consumer(s)
 */
class ParameterQueue
{
public:
    /**
     * @brief Constructor
     * @param[in] max_queue_size Maximum queue size
     */
    explicit ParameterQueue(uint32_t max_queue_size);

    /**
     * @brief Push equation parameters to the queue
     * @details If queue is full (its size is equal to @ref m_max_queue_size), this method will
     * block until at least one element has been released.
     * @param[in] params Equation parameters
     */
    void push(const EquationParams& params);

    /**
     * @brief Pop equation parameters from the queue
     * @details It blocks execution until there is new data or an indication that the producer
     * finished queue populating.
     * @param[out] params Equation parameters
     * @returns `true` if element has been popped
     * @returns `false` if the queue is empty and the producer finished queue populating
     */
    bool pop(EquationParams* params);

    /**
     * @brief Mark that producer finished populating the queue
     * @details This is a signal to all consumers that there won't be any new data, so they can
     * process the rest parameters and finish their work.
     */
    void setDone();

private:
    const uint32_t m_max_queue_size{};   ///< Maximum allowable queue size
    std::queue<EquationParams> m_queue;  ///< Parameters queue
    std::atomic_bool m_done_flag{false}; ///< Indication that there won't be any new data
    std::mutex m_queue_mtx;              ///< Mutex to protect @p m_queue from concurrent access

    std::condition_variable m_not_full;  ///< The queue isn't full condition
    std::condition_variable m_not_empty; ///< The queue isn't empty condition
};

} // namespace SE
