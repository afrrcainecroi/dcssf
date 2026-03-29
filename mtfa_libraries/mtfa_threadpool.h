#pragma once

#include <vector>
#include <map>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>


#include <atomic>
#include <vector>
#include <thread>
#include <memory>
#include <future>
#include <functional>
#include <type_traits>
#include <cassert>

#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <chrono>
#include <utility>
#include <type_traits>
#include <cassert>

#include <mutex>
#include <atomic>
#include <condition_variable>
#include <cassert>



#include <libguile.h>


//Vedi anche http://roar11.com/2016/01/a-platform-independent-thread-pool-using-c14/
#if 0
class mtfa_ThreadPool
{
private:
    // I thread che stanno lavorando
    std::vector< std::thread > workers;

    // La coda delle richieste da esaudire (funzioni)
    std::queue< std::function<void() > > tasks;

    // per sincronizzarsi
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
public:
    mtfa_ThreadPool ( size_t threads )
    {
        stop=false;
        for ( size_t i = 0; i < threads; ++i ) {
            workers.emplace_back (
            [this, i] {
                for ( ;; )
                {
                    std::function<void() > task;
                    {
                        std::unique_lock<std::mutex> lock ( this->queue_mutex );
                        this->condition.wait ( lock, [this] {
                            return this->stop || !this->tasks.empty();
                        } );
                        if ( this->stop && this->tasks.empty() ) {
                            return;
                        }
                        task = std::move ( this->tasks.front() );
                        this->tasks.pop();
                    }

                    task();
                }
            }
            );
        }
    };

    template<class F, class... Args>
    auto enqueue ( F&& f, Args&&... args ) -> std::future<typename std::result_of<F ( Args... ) >::type> {
        using return_type = typename std::result_of < F ( Args... ) >::type;

        auto task = std::make_shared < std::packaged_task < return_type() > > (
            std::bind ( std::forward<F> ( f ), std::forward<Args> ( args )... )
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock ( queue_mutex );

            // don't allow enqueueing after stopping the pool
            if ( stop )
            {
                throw std::runtime_error ( "enqueue on stopped ThreadPool" );
            }

            tasks.emplace ( [task]()
            {
                ( *task ) ();
            } );
        }
        condition.notify_one();
        return res;
    };
    
    ~mtfa_ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock ( queue_mutex );
            stop = true;
        }
        condition.notify_all();
        for ( std::thread &worker : workers ) {
            worker.join();
        }
    };

    size_t size()
    {
        return tasks.size();
    }
};
#elif 1
#pragma once

/**
 * @file BS_thread_pool.hpp
 * @author Barak Shoshany (baraksh@gmail.com) (http://baraksh.com)
 * @version 3.3.0
 * @date 2022-08-03
 * @copyright Copyright (c) 2022 Barak Shoshany. Licensed under the MIT license. If you found this project useful, please consider starring it on GitHub! If you use this library in software of any kind, please provide a link to the GitHub repository https://github.com/bshoshany/thread-pool in the source code and documentation. If you use this library in published research, please cite it as follows: Barak Shoshany, "A C++17 Thread Pool for High-Performance Scientific Computing", doi:10.5281/zenodo.4742687, arXiv:2105.00613 (May 2021)
 *
 * @brief BS::thread_pool: a fast, lightweight, and easy-to-use C++17 thread pool library. This header file contains the entire library, including the main BS::thread_pool class and the helper classes BS::multi_future, BS::blocks, BS:synced_stream, and BS::timer.
 */

#define BS_THREAD_POOL_VERSION "v3.3.0 (2022-08-03)"

#include <atomic>             // std::atomic
#include <chrono>             // std::chrono
#include <condition_variable> // std::condition_variable
#include <exception>          // std::current_exception
#include <functional>         // std::bind, std::function, std::invoke
#include <future>             // std::future, std::promise
#include <iostream>           // std::cout, std::endl, std::flush, std::ostream
#include <memory>             // std::make_shared, std::make_unique, std::shared_ptr, std::unique_ptr
#include <mutex>              // std::mutex, std::scoped_lock, std::unique_lock
#include <queue>              // std::queue
#include <thread>             // std::thread
#include <type_traits>        // std::common_type_t, std::conditional_t, std::decay_t, std::invoke_result_t, std::is_void_v
#include <utility>            // std::forward, std::move, std::swap
#include <vector>             // std::vector

/**
 * @brief A convenient shorthand for the type of std::thread::hardware_concurrency(). Should evaluate to unsigned int.
 */
using concurrency_t = std::invoke_result_t<decltype(std::thread::hardware_concurrency)>;

// ============================================================================================= //
//                                    Begin class multi_future                                   //

/**
 * @brief A helper class to facilitate waiting for and/or getting the results of multiple futures at once.
 *
 * @tparam T The return type of the futures.
 */
template <typename T>
class [[nodiscard]] multi_future
{
public:
    /**
     * @brief Construct a multi_future object with the given number of futures.
     *
     * @param num_futures_ The desired number of futures to store.
     */
    multi_future(const size_t num_futures_ = 0) : futures(num_futures_) {}

    /**
     * @brief Get the results from all the futures stored in this multi_future object, rethrowing any stored exceptions.
     *
     * @return If the futures return void, this function returns void as well. Otherwise, it returns a vector containing the results.
     */
    [[nodiscard]] std::conditional_t<std::is_void_v<T>, void, std::vector<T>> get()
    {
        if constexpr (std::is_void_v<T>)
        {
            for (size_t i = 0; i < futures.size(); ++i)
                futures[i].get();
            return;
        }
        else
        {
            std::vector<T> results(futures.size());
            for (size_t i = 0; i < futures.size(); ++i)
                results[i] = futures[i].get();
            return results;
        }
    }

    /**
     * @brief Get a reference to one of the futures stored in this multi_future object.
     *
     * @param i The index of the desired future.
     * @return The future.
     */
    [[nodiscard]] std::future<T>& operator[](const size_t i)
    {
        return futures[i];
    }

    /**
     * @brief Append a future to this multi_future object.
     *
     * @param future The future to append.
     */
    void push_back(std::future<T> future)
    {
        futures.push_back(std::move(future));
    }

    /**
     * @brief Get the number of futures stored in this multi_future object.
     *
     * @return The number of futures.
     */
    [[nodiscard]] size_t size() const
    {
        return futures.size();
    }

    /**
     * @brief Wait for all the futures stored in this multi_future object.
     */
    void wait() const
    {
        for (size_t i = 0; i < futures.size(); ++i)
            futures[i].wait();
    }

private:
    /**
     * @brief A vector to store the futures.
     */
    std::vector<std::future<T>> futures;
};

//                                     End class multi_future                                    //
// ============================================================================================= //

// ============================================================================================= //
//                                       Begin class blocks                                      //

/**
 * @brief A helper class to divide a range into blocks. Used by parallelize_loop() and push_loop().
 *
 * @tparam T1 The type of the first index in the range. Should be a signed or unsigned integer.
 * @tparam T2 The type of the index after the last index in the range. Should be a signed or unsigned integer. If T1 is not the same as T2, a common type will be automatically inferred.
 * @tparam T The common type of T1 and T2.
 */
template <typename T1, typename T2, typename T = std::common_type_t<T1, T2>>
class [[nodiscard]] blocks
{
public:
    /**
     * @brief Construct a blocks object with the given specifications.
     *
     * @param first_index_ The first index in the range.
     * @param index_after_last_ The index after the last index in the range.
     * @param num_blocks_ The desired number of blocks to divide the range into.
     */
    blocks(const T1 first_index_, const T2 index_after_last_, const size_t num_blocks_) : first_index(static_cast<T>(first_index_)), index_after_last(static_cast<T>(index_after_last_)), num_blocks(num_blocks_)
    {
        if (index_after_last < first_index)
            std::swap(index_after_last, first_index);
        total_size = static_cast<size_t>(index_after_last - first_index);
        block_size = static_cast<size_t>(total_size / num_blocks);
        if (block_size == 0)
        {
            block_size = 1;
            num_blocks = (total_size > 1) ? total_size : 1;
        }
    }

    /**
     * @brief Get the first index of a block.
     *
     * @param i The block number.
     * @return The first index.
     */
    [[nodiscard]] T start(const size_t i) const
    {
        return static_cast<T>(i * block_size) + first_index;
    }

    /**
     * @brief Get the index after the last index of a block.
     *
     * @param i The block number.
     * @return The index after the last index.
     */
    [[nodiscard]] T end(const size_t i) const
    {
        return (i == num_blocks - 1) ? index_after_last : (static_cast<T>((i + 1) * block_size) + first_index);
    }

    /**
     * @brief Get the number of blocks. Note that this may be different than the desired number of blocks that was passed to the constructor.
     *
     * @return The number of blocks.
     */
    [[nodiscard]] size_t get_num_blocks() const
    {
        return num_blocks;
    }

    /**
     * @brief Get the total number of indices in the range.
     *
     * @return The total number of indices.
     */
    [[nodiscard]] size_t get_total_size() const
    {
        return total_size;
    }

private:
    /**
     * @brief The size of each block (except possibly the last block).
     */
    size_t block_size = 0;

    /**
     * @brief The first index in the range.
     */
    T first_index = 0;

    /**
     * @brief The index after the last index in the range.
     */
    T index_after_last = 0;

    /**
     * @brief The number of blocks.
     */
    size_t num_blocks = 0;

    /**
     * @brief The total number of indices in the range.
     */
    size_t total_size = 0;
};

//                                        End class blocks                                       //
// ============================================================================================= //

// ============================================================================================= //
//                                    Begin class thread_pool                                    //

/**
 * @brief A fast, lightweight, and easy-to-use C++17 thread pool class.
 */
class [[nodiscard]] mtfa_ThreadPool
{
public:
    // ============================
    // Constructors and destructors
    // ============================

    /**
     * @brief Construct a new thread pool.
     *
     * @param thread_count_ The number of threads to use. The default value is the total number of hardware threads available, as reported by the implementation. This is usually determined by the number of cores in the CPU. If a core is hyperthreaded, it will count as two threads.
     */
    mtfa_ThreadPool(const concurrency_t thread_count_ = 0) : thread_count(determine_thread_count(thread_count_)), threads(std::make_unique<std::thread[]>(determine_thread_count(thread_count_)))
    {
        create_threads();
    }

    /**
     * @brief Destruct the thread pool. Waits for all tasks to complete, then destroys all threads. Note that if the pool is paused, then any tasks still in the queue will never be executed.
     */
    ~mtfa_ThreadPool()
    {
        wait_for_tasks();
        destroy_threads();
    }

    // =======================
    // Public member functions
    // =======================

    /**
     * @brief Get the number of tasks currently waiting in the queue to be executed by the threads.
     *
     * @return The number of queued tasks.
     */
    [[nodiscard]] size_t get_tasks_queued() const
    {
        const std::scoped_lock tasks_lock(tasks_mutex);
        return tasks.size();
    }

    /**
     * @brief Get the number of tasks currently being executed by the threads.
     *
     * @return The number of running tasks.
     */
    [[nodiscard]] size_t get_tasks_running() const
    {
        const std::scoped_lock tasks_lock(tasks_mutex);
        return tasks_total - tasks.size();
    }

    /**
     * @brief Get the total number of unfinished tasks: either still in the queue, or running in a thread. Note that get_tasks_total() == get_tasks_queued() + get_tasks_running().
     *
     * @return The total number of tasks.
     */
    [[nodiscard]] size_t get_tasks_total() const
    {
        return tasks_total;
    }

    /**
     * @brief Get the number of threads in the pool.
     *
     * @return The number of threads.
     */
    [[nodiscard]] concurrency_t get_thread_count() const
    {
        return thread_count;
    }

    /**
     * @brief Check whether the pool is currently paused.
     *
     * @return true if the pool is paused, false if it is not paused.
     */
    [[nodiscard]] bool is_paused() const
    {
        return paused;
    }

    /**
     * @brief Parallelize a loop by automatically splitting it into blocks and submitting each block separately to the queue. Returns a multi_future object that contains the futures for all of the blocks.
     *
     * @tparam F The type of the function to loop through.
     * @tparam T1 The type of the first index in the loop. Should be a signed or unsigned integer.
     * @tparam T2 The type of the index after the last index in the loop. Should be a signed or unsigned integer. If T1 is not the same as T2, a common type will be automatically inferred.
     * @tparam T The common type of T1 and T2.
     * @tparam R The return value of the loop function F (can be void).
     * @param first_index The first index in the loop.
     * @param index_after_last The index after the last index in the loop. The loop will iterate from first_index to (index_after_last - 1) inclusive. In other words, it will be equivalent to "for (T i = first_index; i < index_after_last; ++i)". Note that if index_after_last == first_index, no blocks will be submitted.
     * @param loop The function to loop through. Will be called once per block. Should take exactly two arguments: the first index in the block and the index after the last index in the block. loop(start, end) should typically involve a loop of the form "for (T i = start; i < end; ++i)".
     * @param num_blocks The maximum number of blocks to split the loop into. The default is to use the number of threads in the pool.
     * @return A multi_future object that can be used to wait for all the blocks to finish. If the loop function returns a value, the multi_future object can also be used to obtain the values returned by each block.
     */
    template <typename F, typename T1, typename T2, typename T = std::common_type_t<T1, T2>, typename R = std::invoke_result_t<std::decay_t<F>, T, T>>
    [[nodiscard]] multi_future<R> parallelize_loop(const T1 first_index, const T2 index_after_last, F&& loop, const size_t num_blocks = 0)
    {
        blocks blks(first_index, index_after_last, num_blocks ? num_blocks : thread_count);
        if (blks.get_total_size() > 0)
        {
            multi_future<R> mf(blks.get_num_blocks());
            for (size_t i = 0; i < blks.get_num_blocks(); ++i)
                mf[i] = submit(std::forward<F>(loop), blks.start(i), blks.end(i));
            return mf;
        }
        else
        {
            return multi_future<R>();
        }
    }

    /**
     * @brief Parallelize a loop by automatically splitting it into blocks and submitting each block separately to the queue. Returns a multi_future object that contains the futures for all of the blocks. This overload is used for the special case where the first index is 0.
     *
     * @tparam F The type of the function to loop through.
     * @tparam T The type of the loop indices. Should be a signed or unsigned integer.
     * @tparam R The return value of the loop function F (can be void).
     * @param index_after_last The index after the last index in the loop. The loop will iterate from 0 to (index_after_last - 1) inclusive. In other words, it will be equivalent to "for (T i = 0; i < index_after_last; ++i)". Note that if index_after_last == 0, no blocks will be submitted.
     * @param loop The function to loop through. Will be called once per block. Should take exactly two arguments: the first index in the block and the index after the last index in the block. loop(start, end) should typically involve a loop of the form "for (T i = start; i < end; ++i)".
     * @param num_blocks The maximum number of blocks to split the loop into. The default is to use the number of threads in the pool.
     * @return A multi_future object that can be used to wait for all the blocks to finish. If the loop function returns a value, the multi_future object can also be used to obtain the values returned by each block.
     */
    template <typename F, typename T, typename R = std::invoke_result_t<std::decay_t<F>, T, T>>
    [[nodiscard]] multi_future<R> parallelize_loop(const T index_after_last, F&& loop, const size_t num_blocks = 0)
    {
        return parallelize_loop(0, index_after_last, std::forward<F>(loop), num_blocks);
    }

    /**
     * @brief Pause the pool. The workers will temporarily stop retrieving new tasks out of the queue, although any tasks already executed will keep running until they are finished.
     */
    void pause()
    {
        paused = true;
    }

    /**
     * @brief Parallelize a loop by automatically splitting it into blocks and submitting each block separately to the queue. Does not return a multi_future, so the user must use wait_for_tasks() or some other method to ensure that the loop finishes executing, otherwise bad things will happen.
     *
     * @tparam F The type of the function to loop through.
     * @tparam T1 The type of the first index in the loop. Should be a signed or unsigned integer.
     * @tparam T2 The type of the index after the last index in the loop. Should be a signed or unsigned integer. If T1 is not the same as T2, a common type will be automatically inferred.
     * @tparam T The common type of T1 and T2.
     * @param first_index The first index in the loop.
     * @param index_after_last The index after the last index in the loop. The loop will iterate from first_index to (index_after_last - 1) inclusive. In other words, it will be equivalent to "for (T i = first_index; i < index_after_last; ++i)". Note that if index_after_last == first_index, no blocks will be submitted.
     * @param loop The function to loop through. Will be called once per block. Should take exactly two arguments: the first index in the block and the index after the last index in the block. loop(start, end) should typically involve a loop of the form "for (T i = start; i < end; ++i)".
     * @param num_blocks The maximum number of blocks to split the loop into. The default is to use the number of threads in the pool.
     */
    template <typename F, typename T1, typename T2, typename T = std::common_type_t<T1, T2>>
    void push_loop(const T1 first_index, const T2 index_after_last, F&& loop, const size_t num_blocks = 0)
    {
        blocks blks(first_index, index_after_last, num_blocks ? num_blocks : thread_count);
        if (blks.get_total_size() > 0)
        {
            for (size_t i = 0; i < blks.get_num_blocks(); ++i)
                push_task(std::forward<F>(loop), blks.start(i), blks.end(i));
        }
    }

    /**
     * @brief Parallelize a loop by automatically splitting it into blocks and submitting each block separately to the queue. Does not return a multi_future, so the user must use wait_for_tasks() or some other method to ensure that the loop finishes executing, otherwise bad things will happen. This overload is used for the special case where the first index is 0.
     *
     * @tparam F The type of the function to loop through.
     * @tparam T The type of the loop indices. Should be a signed or unsigned integer.
     * @param index_after_last The index after the last index in the loop. The loop will iterate from 0 to (index_after_last - 1) inclusive. In other words, it will be equivalent to "for (T i = 0; i < index_after_last; ++i)". Note that if index_after_last == 0, no blocks will be submitted.
     * @param loop The function to loop through. Will be called once per block. Should take exactly two arguments: the first index in the block and the index after the last index in the block. loop(start, end) should typically involve a loop of the form "for (T i = start; i < end; ++i)".
     * @param num_blocks The maximum number of blocks to split the loop into. The default is to use the number of threads in the pool.
     */
    template <typename F, typename T>
    void push_loop(const T index_after_last, F&& loop, const size_t num_blocks = 0)
    {
        push_loop(0, index_after_last, std::forward<F>(loop), num_blocks);
    }

    /**
     * @brief Push a function with zero or more arguments, but no return value, into the task queue. Does not return a future, so the user must use wait_for_tasks() or some other method to ensure that the task finishes executing, otherwise bad things will happen.
     *
     * @tparam F The type of the function.
     * @tparam A The types of the arguments.
     * @param task The function to push.
     * @param args The zero or more arguments to pass to the function. Note that if the task is a class member function, the first argument must be a pointer to the object, i.e. &object (or this), followed by the actual arguments.
     */
    template <typename F, typename... A>
    void push_task(F&& task, A&&... args)
    {
        std::function<void()> task_function = std::bind(std::forward<F>(task), std::forward<A>(args)...);
        {
            const std::scoped_lock tasks_lock(tasks_mutex);
            tasks.push(task_function);
        }
        ++tasks_total;
        task_available_cv.notify_one();
    }

    /**
     * @brief Reset the number of threads in the pool. Waits for all currently running tasks to be completed, then destroys all threads in the pool and creates a new thread pool with the new number of threads. Any tasks that were waiting in the queue before the pool was reset will then be executed by the new threads. If the pool was paused before resetting it, the new pool will be paused as well.
     *
     * @param thread_count_ The number of threads to use. The default value is the total number of hardware threads available, as reported by the implementation. This is usually determined by the number of cores in the CPU. If a core is hyperthreaded, it will count as two threads.
     */
    void reset(const concurrency_t thread_count_ = 0)
    {
        const bool was_paused = paused;
        paused = true;
        wait_for_tasks();
        destroy_threads();
        thread_count = determine_thread_count(thread_count_);
        threads = std::make_unique<std::thread[]>(thread_count);
        paused = was_paused;
        create_threads();
    }

    /**
     * @brief Submit a function with zero or more arguments into the task queue. If the function has a return value, get a future for the eventual returned value. If the function has no return value, get an std::future<void> which can be used to wait until the task finishes.
     *
     * @tparam F The type of the function.
     * @tparam A The types of the zero or more arguments to pass to the function.
     * @tparam R The return type of the function (can be void).
     * @param task The function to submit.
     * @param args The zero or more arguments to pass to the function. Note that if the task is a class member function, the first argument must be a pointer to the object, i.e. &object (or this), followed by the actual arguments.
     * @return A future to be used later to wait for the function to finish executing and/or obtain its returned value if it has one.
     */
    template <typename F, typename... A, typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<A>...>>
    [[nodiscard]] std::future<R> submit(F&& task, A&&... args)
    {
        std::function<R()> task_function = std::bind(std::forward<F>(task), std::forward<A>(args)...);
        std::shared_ptr<std::promise<R>> task_promise = std::make_shared<std::promise<R>>();
        push_task(
            [task_function, task_promise]
            {
                try
                {
                    if constexpr (std::is_void_v<R>)
                    {
                        std::invoke(task_function);
                        task_promise->set_value();
                    }
                    else
                    {
                        task_promise->set_value(std::invoke(task_function));
                    }
                }
                catch (...)
                {
                    try
                    {
                        task_promise->set_exception(std::current_exception());
                    }
                    catch (...)
                    {
                    }
                }
            });
        return task_promise->get_future();
    }

    /**
     * @brief Unpause the pool. The workers will resume retrieving new tasks out of the queue.
     */
    void unpause()
    {
        paused = false;
    }

    /**
     * @brief Wait for tasks to be completed. Normally, this function waits for all tasks, both those that are currently running in the threads and those that are still waiting in the queue. However, if the pool is paused, this function only waits for the currently running tasks (otherwise it would wait forever). Note: To wait for just one specific task, use submit() instead, and call the wait() member function of the generated future.
     */
    void wait_for_tasks()
    {
        waiting = true;
        std::unique_lock<std::mutex> tasks_lock(tasks_mutex);
        task_done_cv.wait(tasks_lock, [this] { return (tasks_total == (paused ? tasks.size() : 0)); });
        waiting = false;
    }

private:
    // ========================
    // Private member functions
    // ========================

    /**
     * @brief Create the threads in the pool and assign a worker to each thread.
     */
    void create_threads()
    {
        running = true;
        for (concurrency_t i = 0; i < thread_count; ++i)
        {
            threads[i] = std::thread(&mtfa_ThreadPool::worker, this);
        }
    }
	
    /**
     * @brief Destroy the threads in the pool.
     */
    void destroy_threads()
    {
        running = false;
        task_available_cv.notify_all();
        for (concurrency_t i = 0; i < thread_count; ++i)
        {
            threads[i].join();
        }
    }

    /**
     * @brief Determine how many threads the pool should have, based on the parameter passed to the constructor or reset().
     *
     * @param thread_count_ The parameter passed to the constructor or reset(). If the parameter is a positive number, then the pool will be created with this number of threads. If the parameter is non-positive, or a parameter was not supplied (in which case it will have the default value of 0), then the pool will be created with the total number of hardware threads available, as obtained from std::thread::hardware_concurrency(). If the latter returns a non-positive number for some reason, then the pool will be created with just one thread.
     * @return The number of threads to use for constructing the pool.
     */
    [[nodiscard]] concurrency_t determine_thread_count(const concurrency_t thread_count_)
    {
        if (thread_count_ > 0)
            return thread_count_;
        else
        {
            if (std::thread::hardware_concurrency() > 0)
                return std::thread::hardware_concurrency();
            else
                return 1;
        }
    }

    /**
     * @brief A worker function to be assigned to each thread in the pool. Waits until it is notified by push_task() that a task is available, and then retrieves the task from the queue and executes it. Once the task finishes, the worker notifies wait_for_tasks() in case it is waiting.
     */
    void worker()
    {
        scm_init_guile();
        while (running)
        {
            std::function<void()> task;
            std::unique_lock<std::mutex> tasks_lock(tasks_mutex);
            task_available_cv.wait(tasks_lock, [this] { return !tasks.empty() || !running; });
            if (running && !paused)
            {
                task = std::move(tasks.front());
                tasks.pop();
                tasks_lock.unlock();
                task();
                tasks_lock.lock();
                --tasks_total;
                if (waiting)
                    task_done_cv.notify_one();
            }
        }
    }

    // ============
    // Private data
    // ============

    /**
     * @brief An atomic variable indicating whether the workers should pause. When set to true, the workers temporarily stop retrieving new tasks out of the queue, although any tasks already executed will keep running until they are finished. When set to false again, the workers resume retrieving tasks.
     */
    std::atomic<bool> paused = false;

    /**
     * @brief An atomic variable indicating to the workers to keep running. When set to false, the workers permanently stop working.
     */
    std::atomic<bool> running = false;

    /**
     * @brief A condition variable used to notify worker() that a new task has become available.
     */
    std::condition_variable task_available_cv = {};

    /**
     * @brief A condition variable used to notify wait_for_tasks() that a tasks is done.
     */
    std::condition_variable task_done_cv = {};

    /**
     * @brief A queue of tasks to be executed by the threads.
     */
    std::queue<std::function<void()>> tasks = {};

    /**
     * @brief An atomic variable to keep track of the total number of unfinished tasks - either still in the queue, or running in a thread.
     */
    std::atomic<size_t> tasks_total = 0;

    /**
     * @brief A mutex to synchronize access to the task queue by different threads.
     */
    mutable std::mutex tasks_mutex = {};

    /**
     * @brief The number of threads in the pool.
     */
    concurrency_t thread_count = 0;

    /**
     * @brief A smart pointer to manage the memory allocated for the threads.
     */
    std::unique_ptr<std::thread[]> threads = nullptr;

    /**
     * @brief An atomic variable indicating that wait_for_tasks() is active and expects to be notified whenever a task is done.
     */
    std::atomic<bool> waiting = false;
};

//                                     End class thread_pool                                     //
// ============================================================================================= //

// ============================================================================================= //
//                                   Begin class synced_stream                                   //

/**
 * @brief A helper class to synchronize printing to an output stream by different threads.
 */
class [[nodiscard]] synced_stream
{
public:
    /**
     * @brief Construct a new synced stream.
     *
     * @param out_stream_ The output stream to print to. The default value is std::cout.
     */
    synced_stream(std::ostream& out_stream_ = std::cout) : out_stream(out_stream_) {}

    /**
     * @brief Print any number of items into the output stream. Ensures that no other threads print to this stream simultaneously, as long as they all exclusively use the same synced_stream object to print.
     *
     * @tparam T The types of the items
     * @param items The items to print.
     */
    template <typename... T>
    void print(T&&... items)
    {
        const std::scoped_lock lock(stream_mutex);
        (out_stream << ... << std::forward<T>(items));
    }

    /**
     * @brief Print any number of items into the output stream, followed by a newline character. Ensures that no other threads print to this stream simultaneously, as long as they all exclusively use the same synced_stream object to print.
     *
     * @tparam T The types of the items
     * @param items The items to print.
     */
    template <typename... T>
    void println(T&&... items)
    {
        print(std::forward<T>(items)..., '\n');
    }

    /**
     * @brief A stream manipulator to pass to a synced_stream (an explicit cast of std::endl). Prints a newline character to the stream, and then flushes it. Should only be used if flushing is desired, otherwise '\n' should be used instead.
     */
    inline static std::ostream& (&endl)(std::ostream&) = static_cast<std::ostream& (&)(std::ostream&)>(std::endl);

    /**
     * @brief A stream manipulator to pass to a synced_stream (an explicit cast of std::flush). Used to flush the stream.
     */
    inline static std::ostream& (&flush)(std::ostream&) = static_cast<std::ostream& (&)(std::ostream&)>(std::flush);

private:
    /**
     * @brief The output stream to print to.
     */
    std::ostream& out_stream;

    /**
     * @brief A mutex to synchronize printing.
     */
    mutable std::mutex stream_mutex = {};
};

//                                    End class synced_stream                                    //
// ============================================================================================= //

// ============================================================================================= //
//                                       Begin class timer                                       //

/**
 * @brief A helper class to measure execution time for benchmarking purposes.
 */
class [[nodiscard]] timer
{
public:
    /**
     * @brief Start (or restart) measuring time.
     */
    void start()
    {
        start_time = std::chrono::steady_clock::now();
    }

    /**
     * @brief Stop measuring time and store the elapsed time since start().
     */
    void stop()
    {
        elapsed_time = std::chrono::steady_clock::now() - start_time;
    }

    /**
     * @brief Get the number of milliseconds that have elapsed between start() and stop().
     *
     * @return The number of milliseconds.
     */
    [[nodiscard]] std::chrono::milliseconds::rep ms() const
    {
        return (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time)).count();
    }

private:
    /**
     * @brief The time point when measuring started.
     */
    std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();

    /**
     * @brief The duration that has elapsed between start() and stop().
     */
    std::chrono::duration<double> elapsed_time = std::chrono::duration<double>::zero();
};

//                                        End class timer                                        //
// ============================================================================================= //
#else
#pragma once

/**
 * @file thread_pool.hpp
 * @author Barak Shoshany (baraksh@gmail.com) (http://baraksh.com)
 * @version 2.0.0
 * @date 2021-08-14
 * @copyright Copyright (c) 2021 Barak Shoshany. Licensed under the MIT license. If you use this library in published research, please cite it as follows:
 *  - Barak Shoshany, "A C++17 Thread Pool for High-Performance Scientific Computing", doi:10.5281/zenodo.4742687, arXiv:2105.00613 (May 2021)
 *
 * @brief A C++17 thread pool for high-performance scientific computing.
 * @details A modern C++17-compatible thread pool implementation, built from scratch with high-performance scientific computing in mind. The thread pool is implemented as a single lightweight and self-contained class, and does not have any dependencies other than the C++17 standard library, thus allowing a great degree of portability. In particular, this implementation does not utilize OpenMP or any other high-level multithreading APIs, and thus gives the programmer precise low-level control over the details of the parallelization, which permits more robust optimizations. The thread pool was extensively tested on both AMD and Intel CPUs with up to 40 cores and 80 threads. Other features include automatic generation of futures and easy parallelization of loops. Two helper classes enable synchronizing printing to an output stream by different threads and measuring execution time for benchmarking purposes. Please visit the GitHub repository at https://github.com/bshoshany/thread-pool for documentation and updates, or to submit feature requests and bug reports.
 */

#define THREAD_POOL_VERSION "v2.0.0 (2021-08-14)"

#include <atomic>      // std::atomic
#include <chrono>      // std::chrono
#include <cstdint>     // std::int_fast64_t, std::uint_fast32_t
#include <functional>  // std::function
#include <future>      // std::future, std::promise
#include <iostream>    // std::cout, std::ostream
#include <memory>      // std::shared_ptr, std::unique_ptr
#include <mutex>       // std::mutex, std::scoped_lock
#include <queue>       // std::queue
#include <thread>      // std::this_thread, std::thread
#include <type_traits> // std::common_type_t, std::decay_t, std::enable_if_t, std::is_void_v, std::invoke_result_t
#include <utility>     // std::move

// ============================================================================================= //
//                                    Begin class thread_pool                                    //

/**
 * @brief A C++17 thread pool class. The user submits tasks to be executed into a queue. Whenever a thread becomes available, it pops a task from the queue and executes it. Each task is automatically assigned a future, which can be used to wait for the task to finish executing and/or obtain its eventual return value.
 */
class mtfa_ThreadPool
{
    typedef std::uint_fast32_t ui32;
    typedef std::uint_fast64_t ui64;

public:
    // ============================
    // Constructors and destructors
    // ============================

    /**
     * @brief Construct a new thread pool.
     *
     * @param _thread_count The number of threads to use. The default value is the total number of hardware threads available, as reported by the implementation. With a hyperthreaded CPU, this will be twice the number of CPU cores. If the argument is zero, the default value will be used instead.
     */
    mtfa_ThreadPool(const ui32 &_thread_count = std::thread::hardware_concurrency())
        : thread_count(_thread_count ? _thread_count : std::thread::hardware_concurrency()), threads(new std::thread[_thread_count ? _thread_count : std::thread::hardware_concurrency()])
    {
        create_threads();
    }

    /**
     * @brief Destruct the thread pool. Waits for all tasks to complete, then destroys all threads. Note that if the variable paused is set to true, then any tasks still in the queue will never be executed.
     */
    ~mtfa_ThreadPool()
    {
        wait_for_tasks();
        running = false;
        destroy_threads();
    }

    // =======================
    // Public member functions
    // =======================

    /**
     * @brief Get the number of tasks currently waiting in the queue to be executed by the threads.
     *
     * @return The number of queued tasks.
     */
    ui64 get_tasks_queued() const
    {
        const std::scoped_lock lock(queue_mutex);
        return tasks.size();
    }

    /**
     * @brief Get the number of tasks currently being executed by the threads.
     *
     * @return The number of running tasks.
     */
    ui32 get_tasks_running() const
    {
        return tasks_total - (ui32)get_tasks_queued();
    }

    /**
     * @brief Get the total number of unfinished tasks - either still in the queue, or running in a thread.
     *
     * @return The total number of tasks.
     */
    ui32 get_tasks_total() const
    {
        return tasks_total;
    }

    /**
     * @brief Get the number of threads in the pool.
     *
     * @return The number of threads.
     */
    ui32 get_thread_count() const
    {
        return thread_count;
    }

    /**
     * @brief Parallelize a loop by splitting it into blocks, submitting each block separately to the thread pool, and waiting for all blocks to finish executing. The user supplies a loop function, which will be called once per block and should iterate over the block's range.
     *
     * @tparam T1 The type of the first index in the loop. Should be a signed or unsigned integer.
     * @tparam T2 The type of the index after the last index in the loop. Should be a signed or unsigned integer. If T1 is not the same as T2, a common type will be automatically inferred.
     * @tparam F The type of the function to loop through.
     * @param first_index The first index in the loop.
     * @param index_after_last The index after the last index in the loop. The loop will iterate from first_index to (index_after_last - 1) inclusive. In other words, it will be equivalent to "for (T i = first_index; i < index_after_last; i++)". Note that if first_index == index_after_last, the function will terminate without doing anything.
     * @param loop The function to loop through. Will be called once per block. Should take exactly two arguments: the first index in the block and the index after the last index in the block. loop(start, end) should typically involve a loop of the form "for (T i = start; i < end; i++)".
     * @param num_blocks The maximum number of blocks to split the loop into. The default is to use the number of threads in the pool.
     */
    template <typename T1, typename T2, typename F>
    void parallelize_loop(const T1 &first_index, const T2 &index_after_last, const F &loop, ui32 num_blocks = 0)
    {
        typedef std::common_type_t<T1, T2> T;
        T the_first_index = (T)first_index;
        T last_index = (T)index_after_last;
        if (the_first_index == last_index)
            return;
        if (last_index < the_first_index)
        {
            T temp = last_index;
            last_index = the_first_index;
            the_first_index = temp;
        }
        last_index--;
        if (num_blocks == 0)
            num_blocks = thread_count;
        ui64 total_size = (ui64)(last_index - the_first_index + 1);
        ui64 block_size = (ui64)(total_size / num_blocks);
        if (block_size == 0)
        {
            block_size = 1;
            num_blocks = (ui32)total_size > 1 ? (ui32)total_size : 1;
        }
        std::atomic<ui32> blocks_running = 0;
        for (ui32 t = 0; t < num_blocks; t++)
        {
            T start = ((T)(t * block_size) + the_first_index);
            T end = (t == num_blocks - 1) ? last_index + 1 : ((T)((t + 1) * block_size) + the_first_index);
            blocks_running++;
            push_task([start, end, &loop, &blocks_running]
                      {
                          loop(start, end);
                          blocks_running--;
                      });
        }
        while (blocks_running != 0)
        {
            sleep_or_yield();
        }
    }

    /**
     * @brief Push a function with no arguments or return value into the task queue.
     *
     * @tparam F The type of the function.
     * @param task The function to push.
     */
    template <typename F>
    void push_task(const F &task)
    {
        tasks_total++;
        {
            const std::scoped_lock lock(queue_mutex);
            tasks.push(std::function<void()>(task));
        }
    }

    /**
     * @brief Push a function with arguments, but no return value, into the task queue.
     * @details The function is wrapped inside a lambda in order to hide the arguments, as the tasks in the queue must be of type std::function<void()>, so they cannot have any arguments or return value. If no arguments are provided, the other overload will be used, in order to avoid the (slight) overhead of using a lambda.
     *
     * @tparam F The type of the function.
     * @tparam A The types of the arguments.
     * @param task The function to push.
     * @param args The arguments to pass to the function.
     */
    template <typename F, typename... A>
    void push_task(const F &task, const A &...args)
    {
        push_task([task, args...]
                  { task(args...); });
    }

    /**
     * @brief Reset the number of threads in the pool. Waits for all currently running tasks to be completed, then destroys all threads in the pool and creates a new thread pool with the new number of threads. Any tasks that were waiting in the queue before the pool was reset will then be executed by the new threads. If the pool was paused before resetting it, the new pool will be paused as well.
     *
     * @param _thread_count The number of threads to use. The default value is the total number of hardware threads available, as reported by the implementation. With a hyperthreaded CPU, this will be twice the number of CPU cores. If the argument is zero, the default value will be used instead.
     */
    void reset(const ui32 &_thread_count = std::thread::hardware_concurrency())
    {
        bool was_paused = paused;
        paused = true;
        wait_for_tasks();
        running = false;
        destroy_threads();
        thread_count = _thread_count ? _thread_count : std::thread::hardware_concurrency();
        threads.reset(new std::thread[thread_count]);
        paused = was_paused;
        running = true;
        create_threads();
    }

    /**
     * @brief Submit a function with zero or more arguments and no return value into the task queue, and get an std::future<bool> that will be set to true upon completion of the task.
     *
     * @tparam F The type of the function.
     * @tparam A The types of the zero or more arguments to pass to the function.
     * @param task The function to submit.
     * @param args The zero or more arguments to pass to the function.
     * @return A future to be used later to check if the function has finished its execution.
     */
    template <typename F, typename... A, typename = std::enable_if_t<std::is_void_v<std::invoke_result_t<std::decay_t<F>, std::decay_t<A>...>>>>
    std::future<bool> submit(const F &task, const A &...args)
    {
        std::shared_ptr<std::promise<bool>> task_promise(new std::promise<bool>);
        std::future<bool> future = task_promise->get_future();
        push_task([task, args..., task_promise]
                  {
                      try
                      {
                          task(args...);
                          task_promise->set_value(true);
                      }
                      catch (...)
                      {
                          try
                          {
                              task_promise->set_exception(std::current_exception());
                          }
                          catch (...)
                          {
                          }
                      }
                  });
        return future;
    }

    /**
     * @brief Submit a function with zero or more arguments and a return value into the task queue, and get a future for its eventual returned value.
     *
     * @tparam F The type of the function.
     * @tparam A The types of the zero or more arguments to pass to the function.
     * @tparam R The return type of the function.
     * @param task The function to submit.
     * @param args The zero or more arguments to pass to the function.
     * @return A future to be used later to obtain the function's returned value, waiting for it to finish its execution if needed.
     */
    template <typename F, typename... A, typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<A>...>, typename = std::enable_if_t<!std::is_void_v<R>>>
    std::future<R> submit(const F &task, const A &...args)
    {
        std::shared_ptr<std::promise<R>> task_promise(new std::promise<R>);
        std::future<R> future = task_promise->get_future();
        push_task([task, args..., task_promise]
                  {
                      try
                      {
                          task_promise->set_value(task(args...));
                      }
                      catch (...)
                      {
                          try
                          {
                              task_promise->set_exception(std::current_exception());
                          }
                          catch (...)
                          {
                          }
                      }
                  });
        return future;
    }

    /**
     * @brief Wait for tasks to be completed. Normally, this function waits for all tasks, both those that are currently running in the threads and those that are still waiting in the queue. However, if the variable paused is set to true, this function only waits for the currently running tasks (otherwise it would wait forever). To wait for a specific task, use submit() instead, and call the wait() member function of the generated future.
     */
    void wait_for_tasks()
    {
        while (true)
        {
            if (!paused)
            {
                if (tasks_total == 0)
                    break;
            }
            else
            {
                if (get_tasks_running() == 0)
                    break;
            }
            sleep_or_yield();
        }
    }

    // ===========
    // Public data
    // ===========

    /**
     * @brief An atomic variable indicating to the workers to pause. When set to true, the workers temporarily stop popping new tasks out of the queue, although any tasks already executed will keep running until they are done. Set to false again to resume popping tasks.
     */
    std::atomic<bool> paused = false;

    /**
     * @brief The duration, in microseconds, that the worker function should sleep for when it cannot find any tasks in the queue. If set to 0, then instead of sleeping, the worker function will execute std::this_thread::yield() if there are no tasks in the queue. The default value is 1000.
     */
    ui32 sleep_duration = 200; //1000;

private:
    // ========================
    // Private member functions
    // ========================

    /**
     * @brief Create the threads in the pool and assign a worker to each thread.
     */
    void create_threads()
    {
        for (ui32 i = 0; i < thread_count; i++)
        {
            threads[i] = std::thread(&mtfa_ThreadPool::worker, this);
        }
    }

    /**
     * @brief Destroy the threads in the pool by joining them.
     */
    void destroy_threads()
    {
        for (ui32 i = 0; i < thread_count; i++)
        {
            threads[i].join();
        }
    }

    /**
     * @brief Try to pop a new task out of the queue.
     *
     * @param task A reference to the task. Will be populated with a function if the queue is not empty.
     * @return true if a task was found, false if the queue is empty.
     */
    bool pop_task(std::function<void()> &task)
    {
        const std::scoped_lock lock(queue_mutex);
        if (tasks.empty())
            return false;
        else
        {
            task = std::move(tasks.front());
            tasks.pop();
            return true;
        }
    }

    /**
     * @brief Sleep for sleep_duration microseconds. If that variable is set to zero, yield instead.
     *
     */
    void sleep_or_yield()
    {
        if (sleep_duration)
            std::this_thread::sleep_for(std::chrono::microseconds(sleep_duration));
        else
            std::this_thread::yield();
    }

    /**
     * @brief A worker function to be assigned to each thread in the pool. Continuously pops tasks out of the queue and executes them, as long as the atomic variable running is set to true.
     */
    void worker()
    {
        scm_init_guile();
        while (running)
        {
            std::function<void()> task;
            if (!paused && pop_task(task))
            {
                task();
                tasks_total--;
            }
            else
            {
                sleep_or_yield();
            }
        }
    }

    // ============
    // Private data
    // ============

    /**
     * @brief A mutex to synchronize access to the task queue by different threads.
     */
    mutable std::mutex queue_mutex = {};

    /**
     * @brief An atomic variable indicating to the workers to keep running. When set to false, the workers permanently stop working.
     */
    std::atomic<bool> running = true;

    /**
     * @brief A queue of tasks to be executed by the threads.
     */
    std::queue<std::function<void()>> tasks = {};

    /**
     * @brief The number of threads in the pool.
     */
    ui32 thread_count;

    /**
     * @brief A smart pointer to manage the memory allocated for the threads.
     */
    std::unique_ptr<std::thread[]> threads;

    /**
     * @brief An atomic variable to keep track of the total number of unfinished tasks - either still in the queue, or running in a thread.
     */
    std::atomic<ui32> tasks_total = 0;
};

//                                     End class mtfa_ThreadPool                                     //
// ============================================================================================= //

// ============================================================================================= //
//                                   Begin class synced_stream                                   //

/**
 * @brief A helper class to synchronize printing to an output stream by different threads.
 */
class synced_stream
{
public:
    /**
     * @brief Construct a new synced stream.
     *
     * @param _out_stream The output stream to print to. The default value is std::cout.
     */
    synced_stream(std::ostream &_out_stream = std::cout)
        : out_stream(_out_stream){};

    /**
     * @brief Print any number of items into the output stream. Ensures that no other threads print to this stream simultaneously, as long as they all exclusively use this synced_stream object to print.
     *
     * @tparam T The types of the items
     * @param items The items to print.
     */
    template <typename... T>
    void print(const T &...items)
    {
        const std::scoped_lock lock(stream_mutex);
        (out_stream << ... << items);
    }

    /**
     * @brief Print any number of items into the output stream, followed by a newline character. Ensures that no other threads print to this stream simultaneously, as long as they all exclusively use this synced_stream object to print.
     *
     * @tparam T The types of the items
     * @param items The items to print.
     */
    template <typename... T>
    void println(const T &...items)
    {
        print(items..., '\n');
    }

private:
    /**
     * @brief A mutex to synchronize printing.
     */
    mutable std::mutex stream_mutex = {};

    /**
     * @brief The output stream to print to.
     */
    std::ostream &out_stream;
};

//                                    End class synced_stream                                    //
// ============================================================================================= //

// ============================================================================================= //
//                                       Begin class timer                                       //

/**
 * @brief A helper class to measure execution time for benchmarking purposes.
 */
class timer
{
    typedef std::int_fast64_t i64;

public:
    /**
     * @brief Start (or restart) measuring time.
     */
    void start()
    {
        start_time = std::chrono::steady_clock::now();
    }

    /**
     * @brief Stop measuring time and store the elapsed time since start().
     */
    void stop()
    {
        elapsed_time = std::chrono::steady_clock::now() - start_time;
    }

    /**
     * @brief Get the number of milliseconds that have elapsed between start() and stop().
     *
     * @return The number of milliseconds.
     */
    i64 ms() const
    {
        return (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time)).count();
    }

private:
    /**
     * @brief The time point when measuring started.
     */
    std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();

    /**
     * @brief The duration that has elapsed between start() and stop().
     */
    std::chrono::duration<double> elapsed_time = std::chrono::duration<double>::zero();
};

//                                        End class timer                                        //
// ============================================================================================= //

#endif



#if 0
/***********************NEW*****************/

class semaphore
{
public:
	explicit semaphore(unsigned int count = 0) noexcept
	: m_count(count) {}

	void post() noexcept
	{
		{
			std::unique_lock lock(m_mutex);
			++m_count;
		}
		m_cv.notify_one();
	}

	void post(unsigned int count) noexcept
	{
		{
			std::unique_lock lock(m_mutex);
			m_count += count;
		}
		m_cv.notify_all();
	}

	void wait() noexcept
	{
		std::unique_lock lock(m_mutex);
		m_cv.wait(lock, [&]() { return m_count != 0; });
		--m_count;
	}

	template<typename T>
	bool wait_for(T&& t) noexcept
	{
		std::unique_lock lock(m_mutex);
		if(!m_cv.wait_for(lock, t, [&]() { return m_count != 0; })) return false;
		--m_count;
		return true;
	}

	template<typename T>
	bool wait_until(T&& t) noexcept
	{
		std::unique_lock lock(m_mutex);
		if(!m_cv.wait_until(lock, t, [&]() { return m_count != 0; })) return false;
		--m_count;
		return true;
	}

private:
	unsigned int m_count;
	std::mutex m_mutex;
	std::condition_variable m_cv;
};

class fast_semaphore
{
public:
	explicit fast_semaphore(int count = 0) noexcept
	: m_count(count), m_semaphore(0) { assert(count > -1); }

	void post() noexcept
	{
		int count = m_count.fetch_add(1, std::memory_order_release);
		if (count < 0)
			m_semaphore.post();
	}

	void wait() noexcept
	{
		int count = m_count.fetch_sub(1, std::memory_order_acquire);
		if (count < 1)
			m_semaphore.wait();
	}

private:
	std::atomic_int m_count;
	semaphore m_semaphore;
};


template<typename T>
class simple_blocking_queue
{
public:
	template<typename Q = T>
	typename std::enable_if<
		std::is_copy_constructible<Q>::value, void>::type
	push(const T& item)
	{
		{
			std::unique_lock lock(m_mutex);
			m_queue.push(item);
		}
		m_ready.notify_one();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_constructible<Q>::value, void>::type
	push(T&& item)
	{
		{
			std::unique_lock lock(m_mutex);
			m_queue.emplace(std::move(item));
		}
		m_ready.notify_one();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_copy_constructible<Q>::value, bool>::type
	try_push(const T& item)
	{
		{
			std::unique_lock lock(m_mutex, std::try_to_lock);
			if(!lock) return false;
			m_queue.push(item);
		}
		m_ready.notify_one();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_constructible<Q>::value, bool>::type
	try_push(T&& item)
	{
		{
			std::unique_lock lock(m_mutex, std::try_to_lock);
			if(!lock) return false;
			m_queue.emplace(std::move(item));
		}
		m_ready.notify_one();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_copy_assignable<Q>::value and not
		std::is_move_assignable<Q>::value, bool>::type
	pop(T& item)
	{
		std::unique_lock lock(m_mutex);
		while(m_queue.empty() && !m_done) m_ready.wait(lock);
		if(m_queue.empty()) return false;
		item = m_queue.front();
		m_queue.pop();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value, bool>::type
	pop(T& item)
	{
		std::unique_lock lock(m_mutex);
		while(m_queue.empty() && !m_done) m_ready.wait(lock);
		if(m_queue.empty()) return false;
		item = std::move(m_queue.front());
		m_queue.pop();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_copy_assignable<Q>::value and not
		std::is_move_assignable<Q>::value, bool>::type
	try_pop(T& item)
	{
		std::unique_lock lock(m_mutex, std::try_to_lock);
		if(!lock || m_queue.empty()) return false;
		item = m_queue.front();
		m_queue.pop();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value, bool>::type
	try_pop(T& item)
	{
		std::unique_lock lock(m_mutex, std::try_to_lock);
		if(!lock || m_queue.empty()) return false;
		item = std::move(m_queue.front());
		m_queue.pop();
		return true;
	}

	void done() noexcept
	{
		{
			std::unique_lock lock(m_mutex);
			m_done = true;
		}
		m_ready.notify_all();
	}

	bool empty() const noexcept
	{
		std::scoped_lock lock(m_mutex);
		return m_queue.empty();
	}

	unsigned int size() const noexcept
	{
		std::scoped_lock lock(m_mutex);
		return m_queue.size();
	}

private:
	std::queue<T> m_queue;
	std::mutex m_mutex;
	std::condition_variable m_ready;
	bool m_done = false;
};

template<typename T>
class blocking_queue
{
public:
	explicit blocking_queue(unsigned int size)
	: m_size(size), m_pushIndex(0), m_popIndex(0), m_count(0),
	m_data((T*)operator new(size * sizeof(T))),
	m_openSlots(size), m_fullSlots(0)
	{
		assert(size != 0);
	}

	~blocking_queue() noexcept
	{
		while (m_count--)
		{
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
		}
		operator delete(m_data);
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_copy_constructible<Q>::value and
		std::is_nothrow_copy_constructible<Q>::value, void>::type
	push(const T& item) noexcept
	{
		m_openSlots.wait();
		{
			std::scoped_lock lock(m_cs);
			new (m_data + m_pushIndex) T (item);
			m_pushIndex = ++m_pushIndex % m_size;
			++m_count;
		}
		m_fullSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_copy_constructible<Q>::value and not
		std::is_nothrow_copy_constructible<Q>::value, void>::type
	push(const T& item)
	{
		m_openSlots.wait();
		{
			std::scoped_lock lock(m_cs);
			try
			{
				new (m_data + m_pushIndex) T (item);
			}
			catch (...)
			{
				m_openSlots.post();
				throw;
			}
			m_pushIndex = ++m_pushIndex % m_size;
			++m_count;
		}
		m_fullSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_constructible<Q>::value and
		std::is_nothrow_move_constructible<Q>::value, void>::type
	push(T&& item) noexcept
	{
		m_openSlots.wait();
		{
			std::scoped_lock lock(m_cs);
			new (m_data + m_pushIndex) T (std::move(item));
			m_pushIndex = ++m_pushIndex % m_size;
			++m_count;
		}
		m_fullSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_constructible<Q>::value and not
		std::is_nothrow_move_constructible<Q>::value, void>::type
	push(T&& item)
	{
		m_openSlots.wait();
		{
			std::scoped_lock lock(m_cs);
			try
			{
				new (m_data + m_pushIndex) T (std::move(item));
			}
			catch (...)
			{
				m_openSlots.post();
				throw;
			}
			m_pushIndex = ++m_pushIndex % m_size;
			++m_count;
		}
		m_fullSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_copy_constructible<Q>::value and
		std::is_nothrow_copy_constructible<Q>::value, bool>::type
	try_push(const T& item) noexcept
	{
		auto result = m_openSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;
		{
			std::scoped_lock lock(m_cs);
			new (m_data + m_pushIndex) T (item);
			m_pushIndex = ++m_pushIndex % m_size;
			++m_count;
		}
		m_fullSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_copy_constructible<Q>::value and not
		std::is_nothrow_copy_constructible<Q>::value, bool>::type
	try_push(const T& item)
	{
		auto result = m_openSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;
		{
			std::scoped_lock lock(m_cs);
			try
			{
				new (m_data + m_pushIndex) T (item);
			}
			catch (...)
			{
				m_openSlots.post();
				throw;
			}
			m_pushIndex = ++m_pushIndex % m_size;
			++m_count;
		}
		m_fullSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_constructible<Q>::value and
		std::is_nothrow_move_constructible<Q>::value, bool>::type
	try_push(T&& item) noexcept
	{
		auto result = m_openSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;
		{
			std::scoped_lock lock(m_cs);
			new (m_data + m_pushIndex) T (std::move(item));
			m_pushIndex = ++m_pushIndex % m_size;
			++m_count;
		}
		m_fullSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_constructible<Q>::value and not
		std::is_nothrow_move_constructible<Q>::value, bool>::type
	try_push(T&& item)
	{
		auto result = m_openSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;
		{
			std::scoped_lock lock(m_cs);
			try
			{
				new (m_data + m_pushIndex) T (std::move(item));
			}
			catch (...)
			{
				m_openSlots.post();
				throw;
			}
			m_pushIndex = ++m_pushIndex % m_size;
			++m_count;
		}
		m_fullSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
		not std::is_move_assignable<Q>::value and
		std::is_nothrow_copy_assignable<Q>::value, void>::type
	pop(T& item) noexcept
	{
		m_fullSlots.wait();
		{
			std::scoped_lock lock(m_cs);
			item = m_data[m_popIndex];
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
			--m_count;
		}
		m_openSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		not std::is_move_assignable<Q>::value and not
		std::is_nothrow_copy_assignable<Q>::value, void>::type
	pop(T& item)
	{
		m_fullSlots.wait();
		{
			std::scoped_lock lock(m_cs);
			try
			{
				item = m_data[m_popIndex];
			}
			catch (...)
			{
				m_fullSlots.post();
				throw;
			}
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
			--m_count;
		}
		m_openSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value and
		std::is_nothrow_move_assignable<Q>::value, void>::type
	pop(T& item) noexcept
	{
		m_fullSlots.wait();
		{
			std::scoped_lock lock(m_cs);
			item = std::move(m_data[m_popIndex]);
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
			--m_count;
		}
		m_openSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value and not
		std::is_nothrow_move_assignable<Q>::value, void>::type
	pop(T& item)
	{
		m_fullSlots.wait();
		{
			std::scoped_lock lock(m_cs);
			try
			{
				item = std::move(m_data[m_popIndex]);
			}
			catch (...)
			{
				m_fullSlots.post();
				throw;
			}
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
			--m_count;
		}
		m_openSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		not std::is_move_assignable<Q>::value and
		std::is_nothrow_copy_assignable<Q>::value, bool>::type
	try_pop(T& item) noexcept
	{
		auto result = m_fullSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;
		{
			std::scoped_lock lock(m_cs);
			item = m_data[m_popIndex];
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
			--m_count;
		}
		m_openSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
		not std::is_move_assignable<Q>::value and not
		std::is_nothrow_copy_assignable<Q>::value, bool>::type
	try_pop(T& item)
	{
		auto result = m_fullSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;
		{
			std::scoped_lock lock(m_cs);
			try
			{
				item = m_data[m_popIndex];
			}
			catch (...)
			{
				m_fullSlots.post();
				throw;
			}
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
			--m_count;
		}
		m_openSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value and
		std::is_nothrow_move_assignable<Q>::value, bool>::type
	try_pop(T& item) noexcept
	{
		auto result = m_fullSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;
		{
			std::scoped_lock lock(m_cs);
			item = std::move(m_data[m_popIndex]);
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
			--m_count;
		}
		m_openSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value and not
		std::is_nothrow_move_assignable<Q>::value, bool>::type
	try_pop(T& item)
	{
		auto result = m_fullSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;
		{
			std::scoped_lock lock(m_cs);
			try
			{
				item = std::move(m_data[m_popIndex]);
			}
			catch (...)
			{
				m_fullSlots.post();
				throw;
			}
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
			--m_count;
		}
		m_openSlots.post();
		return true;
	}

	bool empty() const noexcept
	{
		std::scoped_lock lock(m_cs);
		return m_count == 0;
	}

	bool full() const noexcept
	{
		std::scoped_lock lock(m_cs);
		return m_count == m_size;
	}

	unsigned int size() const noexcept
	{
		std::scoped_lock lock(m_cs);
		return m_count;
	}

	unsigned int capacity() const noexcept
	{
		return m_size;
	}

private:
	const unsigned int m_size;
	unsigned int m_pushIndex;
	unsigned int m_popIndex;
	unsigned int m_count;
	T* m_data;

	semaphore m_openSlots;
	semaphore m_fullSlots;
	std::mutex m_cs;
};

template<typename T>
class atomic_blocking_queue
{
public:
	explicit atomic_blocking_queue(unsigned int size)
	: m_size(size), m_pushIndex(0), m_popIndex(0), m_count(0),
	m_data((T*)operator new(size * sizeof(T))),
	m_openSlots(size), m_fullSlots(0)
	{
		assert(size != 0);
	}

	~atomic_blocking_queue() noexcept
	{
		while (m_count--)
		{
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
		}
		operator delete(m_data);
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_nothrow_copy_constructible<Q>::value, void>::type
	push(const T& item) noexcept
	{
		m_openSlots.wait();

		auto pushIndex = m_pushIndex.fetch_add(1);
		new (m_data + (pushIndex % m_size)) T (item);
		++m_count;

		auto expected = m_pushIndex.load();
		while(!m_pushIndex.compare_exchange_weak(expected, m_pushIndex % m_size))
			expected = m_pushIndex.load();

		m_fullSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_nothrow_move_constructible<Q>::value, void>::type
	push(T&& item) noexcept
	{
		m_openSlots.wait();

		auto pushIndex = m_pushIndex.fetch_add(1);
		new (m_data + (pushIndex % m_size)) T (std::move(item));
		++m_count;

		auto expected = m_pushIndex.load();
		while(!m_pushIndex.compare_exchange_weak(expected, m_pushIndex % m_size))
			expected = m_pushIndex.load();

		m_fullSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_nothrow_copy_constructible<Q>::value, bool>::type
	try_push(const T& item) noexcept
	{
		auto result = m_openSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;

		auto pushIndex = m_pushIndex.fetch_add(1);
		new (m_data + (pushIndex % m_size)) T (item);
		++m_count;

		auto expected = m_pushIndex.load();
		while(!m_pushIndex.compare_exchange_weak(expected, m_pushIndex % m_size))
			expected = m_pushIndex.load();

		m_fullSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_nothrow_move_constructible<Q>::value, bool>::type
	try_push(T&& item) noexcept
	{
		auto result = m_openSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;

		auto pushIndex = m_pushIndex.fetch_add(1);
		new (m_data + (pushIndex % m_size)) T (std::move(item));
		++m_count;

		auto expected = m_pushIndex.load();
		while(!m_pushIndex.compare_exchange_weak(expected, m_pushIndex % m_size))
			expected = m_pushIndex.load();

		m_fullSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
		not std::is_move_assignable<Q>::value and
		std::is_nothrow_copy_assignable<Q>::value, void>::type
	pop(T& item) noexcept
	{
		m_fullSlots.wait();

		auto popIndex = m_popIndex.fetch_add(1);
		item = m_data[popIndex % m_size];
		m_data[popIndex % m_size].~T();
		--m_count;

		auto expected = m_popIndex.load();
		while(!m_popIndex.compare_exchange_weak(expected, m_popIndex % m_size))
			expected = m_popIndex.load();

		m_openSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value and
		std::is_nothrow_move_assignable<Q>::value, void>::type
	pop(T& item) noexcept
	{
		m_fullSlots.wait();

		auto popIndex = m_popIndex.fetch_add(1);
		item = std::move(m_data[popIndex % m_size]);
		m_data[popIndex % m_size].~T();
		--m_count;

		auto expected = m_popIndex.load();
		while(!m_popIndex.compare_exchange_weak(expected, m_popIndex % m_size))
			expected = m_popIndex.load();

		m_openSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		not std::is_move_assignable<Q>::value and
		std::is_nothrow_copy_assignable<Q>::value, bool>::type
	try_pop(T& item) noexcept
	{
		auto result = m_fullSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;

		auto popIndex = m_popIndex.fetch_add(1);
		item = m_data[popIndex % m_size];
		m_data[popIndex % m_size].~T();
		--m_count;

		auto expected = m_popIndex.load();
		while(!m_popIndex.compare_exchange_weak(expected, m_popIndex % m_size))
			expected = m_popIndex.load();

		m_openSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value and
		std::is_nothrow_move_assignable<Q>::value, bool>::type
	try_pop(T& item) noexcept
	{
		auto result = m_fullSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;

		auto popIndex = m_popIndex.fetch_add(1);
		item = std::move(m_data[popIndex % m_size]);
		m_data[popIndex % m_size].~T();
		--m_count;

		auto expected = m_popIndex.load();
		while(!m_popIndex.compare_exchange_weak(expected, m_popIndex % m_size))
			expected = m_popIndex.load();

		m_openSlots.post();
		return true;
	}

	bool empty() const noexcept
	{
		return m_count == 0;
	}

	bool full() const noexcept
	{
		return m_count == m_size;
	}

	unsigned int size() const noexcept
	{
		return m_count;
	}

	unsigned int capacity() const noexcept
	{
		return m_size;
	}

private:
	const unsigned int m_size;
	std::atomic_uint m_pushIndex;
	std::atomic_uint m_popIndex;
	std::atomic_uint m_count;
	T* m_data;

	semaphore m_openSlots;
	semaphore m_fullSlots;
};


class mtfa_thread_pool_x
{
public:
	explicit mtfa_thread_pool_x(unsigned int threads = std::thread::hardware_concurrency())
	: m_queues(threads), m_count(threads)
	{
		assert(threads != 0);
		auto worker = [this](unsigned int i)
		{
			while(true)
			{
				Proc f;
				for(unsigned int n = 0; n < m_count; ++n)
					if(m_queues[(i + n) % m_count].try_pop(f)) break;
				if(!f && !m_queues[i].pop(f)) break;
				f();
			}
		};
		for(unsigned int i = 0; i < threads; ++i)
			m_threads.emplace_back(worker, i);
	}

	~mtfa_thread_pool_x() noexcept
	{
		for(auto& queue : m_queues)
			queue.done();
		for(auto& thread : m_threads)
			thread.join();
	}

	template<typename F, typename... Args>
	void enqueue_work(F&& f, Args&&... args)
	{
		auto work = [f,args...]() { f(args...); };
		unsigned int i = m_index++;
		for(unsigned int n = 0; n < m_count * K; ++n)
			if(m_queues[(i + n) % m_count].try_push(work)) return;
		m_queues[i % m_count].push(work);
	}

	template<typename F, typename... Args>
	auto enqueue_task(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
	{
		using return_type = typename std::result_of<F(Args...)>::type;
		auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		std::future<return_type> res = task->get_future();

		auto work = [task](){ (*task)(); };
		unsigned int i = m_index++;
		for(unsigned int n = 0; n < m_count * K; ++n)
			if(m_queues[(i + n) % m_count].try_push(work)) return res;
		m_queues[i % m_count].push(work);

		return res;
	}

private:
	using Proc = std::function<void(void)>;
	using Queues = std::vector<simple_blocking_queue<Proc>>;
	Queues m_queues;

	using Threads = std::vector<std::thread>;
	Threads m_threads;

	const unsigned int m_count;
	std::atomic_uint m_index = 0;

	inline static const unsigned int K = 2;
};

#endif
