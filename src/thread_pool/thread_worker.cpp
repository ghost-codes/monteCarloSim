#include "thread_worker.h"
#include "thread_pool.h"
#include <iostream>
#include <mutex>

ThreadWorker::ThreadWorker(ThreadPool *pool) : d_thread_pool(pool) {}

void ThreadWorker::operator()() {
  std::unique_lock<std::mutex> lock(d_thread_pool->mutex);
  while (!d_thread_pool->shutdown_requested || !d_thread_pool->queue.empty()) {
    d_thread_pool->busy_threads--;
    d_thread_pool->conditional_variable.wait(lock, [this] {
      return this->d_thread_pool->shutdown_requested ||
             !this->d_thread_pool->queue.empty();
    });

    d_thread_pool->busy_threads++;

    if (!this->d_thread_pool->queue.empty()) {
      auto func = d_thread_pool->queue.front();
      d_thread_pool->queue.pop();

      lock.unlock();
      // std::cout << "Start ..." << std::endl;
      func();
      // std::cout << "End ..." << std::endl;
      lock.lock();
    }
  }
}