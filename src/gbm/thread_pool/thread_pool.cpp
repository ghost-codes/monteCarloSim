#include "thread_pool.h"
#include "thread_worker.h"
#include <cstddef>
#include <mutex>
#include <thread>

ThreadPool::ThreadPool(int size)
    : busy_threads(size), threads(size), shutdown_requested(false) {
  for (int i = 0; i < size; i++) {
    threads[i] = std::thread(ThreadWorker(this));
  }
}

ThreadPool::~ThreadPool() { Shutdown(); }

void ThreadPool::Shutdown() {
  {
    std::lock_guard<std::mutex> lock(mutex);
    shutdown_requested = true;
    conditional_variable.notify_all();
  }

  for (size_t i = 0; i < threads.size(); ++i) {
    if (threads[i].joinable()) {
      threads[i].join();
    }
  }
}

int ThreadPool::QueueSize() {
  std::lock_guard<std::mutex> lock(mutex);
  return queue.size();
}
