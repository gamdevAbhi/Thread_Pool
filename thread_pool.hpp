#ifndef ABSTD_THREAD_POOL
#define ABSTD_THREAD_POOL

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <future>
#include <optional>

namespace abstd
{
    class thread_pool
    {
        public:
        static thread_pool* get_thread_pool();
        static void destroy(bool should_wait = true);

        void terminate();
        void wait();
        
        int get_active_task_count();
        int get_queue_task_count();

        template<typename Func, typename... Args>
        inline auto add_call(Func&& f, Args&&... args) -> std::optional<std::future<typename std::invoke_result_t<Func, Args...>>>
        {
            using return_type = typename std::invoke_result_t<Func, Args...>;

            std::shared_ptr<std::packaged_task<return_type()>> call_wrapper = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
            std::future<return_type> future = call_wrapper->get_future();

            std::function<void()> task =  [call_wrapper] () {(*call_wrapper)();};

            if(try_add_task(task)) return future;
            else return std::nullopt;
        }

        template<typename... Tasks>
        inline auto add_calls(Tasks&&... tasks)
        {
            return std::tuple(add_call(std::forward<Tasks>(tasks))...);
        }
        
        private:
        inline static thread_pool* instance = nullptr;
        inline static std::mutex instance_mtx;
        
        uint32_t thread_size;
        uint32_t active_tasks;
        bool should_stop;
        std::mutex mtx;
        std::condition_variable cv;
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks;
        
        thread_pool();
        ~thread_pool();
        void run_thread(int thread_index);
        bool try_add_task(std::function<void()> task);
        
    };
}

#endif