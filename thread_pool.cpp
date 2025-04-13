#include "thread_pool.hpp"

abstd::thread_pool::thread_pool() :
thread_size(std::thread::hardware_concurrency()),
active_tasks(0),
should_stop(false)
{
    for(int i = 0; i < thread_size; i++) threads.emplace_back(&thread_pool::run_thread, this, i);
}

abstd::thread_pool::~thread_pool()
{
    if(!should_stop) terminate();
}

abstd::thread_pool* abstd::thread_pool::get_thread_pool()
{
    std::unique_lock<std::mutex> lock(instance_mtx);

    if(!instance) instance = new thread_pool();

    return instance;
}

void abstd::thread_pool::run_thread(int thread_index)
{
    while(true)
    {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(mtx);

            cv.wait(lock, [this] () {return !this->tasks.empty() || this->should_stop;});

            if(should_stop) return;

            task = std::move(tasks.front());
            tasks.pop();

            active_tasks++;
        }

        if(task)
        {
            try
            {
                task();
            }
            catch(const std::exception& ex)
            {
                std::cerr << "thread " << thread_index << " tried to run a task. but got an exception : " <<
                ex.what() << '\n';
            }
        }

        {
            std::unique_lock<std::mutex> lock(mtx);

            active_tasks--;

            cv.notify_all();
        }
    }
}

bool abstd::thread_pool::try_add_task(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(mtx);
        
        if(should_stop) return false;

        tasks.push(task);
    }

    cv.notify_one();

    return true;
}

void abstd::thread_pool::terminate()
{
    {
        std::unique_lock<std::mutex> lock(mtx);
        if(should_stop) return;
        
        should_stop = true;

        std::queue<std::function<void()>>().swap(tasks);
    }

    cv.notify_all();
    
    for(std::thread& thread : threads) thread.join();
}

void abstd::thread_pool::wait()
{
    {
        std::unique_lock<std::mutex> lock(mtx);
        if(should_stop) return;
    }

    {
        std::unique_lock<std::mutex> lock(mtx);

        cv.wait(lock, [this] () {return this->tasks.empty() && active_tasks == 0;});
        
        should_stop = true;
    }

    cv.notify_all();

    for(std::thread& thread : threads) thread.join();
}

int abstd::thread_pool::get_active_task_count()
{
    std::unique_lock<std::mutex> lock(mtx);

    return active_tasks;
}

int abstd::thread_pool::get_queue_task_count()
{
    std::unique_lock<std::mutex> lock(mtx);

    return tasks.size();
}

void abstd::thread_pool::destroy(bool should_wait)
{
    std::unique_lock<std::mutex> lock(instance_mtx);
    
    if(instance)
    {
        if(!instance->should_stop && should_wait) instance->wait();
        else if(!instance->should_stop && !should_wait) instance->terminate();
        
        delete instance;
        instance = nullptr;
    }
}