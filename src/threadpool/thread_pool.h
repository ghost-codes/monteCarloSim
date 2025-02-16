#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

class ThreadPool {

public:
  mutable std::mutex mutex;
  std::condition_variable conditional_variable;

  std::vector<std::thread> threads;
  bool shutdown_requested;

  int busy_threads;

  std::queue<std::function<void()>> queue;

  template <typename F, typename... Args>
  auto AddTask(F &&f, Args &&...args) -> std::future<decltype(f(args...))> {

    // Create a function with bounded params
    auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

    // encapsulate in shared ptr to enable copying
    auto task_ptr =
        std::make_shared<std::packaged_task<decltype(f(args...))>>(func);

    // wrap function pointer into a lambda
    auto wrapper_func = [task_ptr]() { (*task_ptr)(); };
    {
      std::lock_guard<std::mutex> lock(mutex);
      queue.push(wrapper_func);
      // wake up one thread if it is waiting
      conditional_variable.notify_one();
    }

    // retrun future
    return task_ptr->get_future();
  }
};

#endif