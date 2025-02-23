#ifndef THREAD_WORKER_H
#define THREAD_WORKER_H

#include "thread_pool.h"
class ThreadWorker {
public:
  ThreadWorker(ThreadPool *pool);

  void operator()();

private:
  ThreadPool *d_thread_pool;
};

#endif