#include "consumer_pool.h"
#include "producer.h"

namespace {

/**
 * @brief Get the number of available consumers
 */
unsigned getConsumerCount()
{
    // We need at least 2 threads for consumer and producer
    static constexpr unsigned MIN_THREADS{2};

    // We need to reserve one thread for the producer, so consumer thread count is 'thread_num - 1'
    return std::max(std::thread::hardware_concurrency(), MIN_THREADS) - 1;
}

} // namespace

int main(int argc, char** argv)
{
    const unsigned consumer_count = getConsumerCount();

    /*
     * We use shared pointer to create parameters queue in order to have more control over its
     * lifetime
     */
    auto queue = std::make_shared<SE::ParameterQueue>(consumer_count);

    // Create producer and consumers
    SE::Producer producer{queue};
    SE::ConsumerPool consumers;

    // The first argument is a program name, so we can skip it.
    producer.run(argv + 1, argc - 1);

    // Run 'consumer_count' consumers
    consumers.run(queue, consumer_count);

    // Wait until all data have been processed
    producer.wait();
    consumers.wait();

    return 0;
}
