#include <random>

#include "../thread_pool.hpp"

void create_array()
{
    std::random_device rd;
    std::mt19937 gen(rd());

    size_t size = gen() % 1000000;
    int32_t* array = new int32_t[size];

    for(int i = 0; i < size; i++)
    {
        array[i] = gen() % INT_MAX;
    }

    delete[] array;
}

int main()
{
    auto thread_pool = abstd::thread_pool::get_thread_pool();

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < 200; i++) thread_pool->add_call(create_array);
    
    thread_pool->wait();

    std::cout << "parallel time (microseconds) : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << std::endl;

    abstd::thread_pool::destroy();

    start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < 200; i++) create_array();

    std::cout << "sequential time (microseconds) : " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() << std::endl;

    return 0;
}