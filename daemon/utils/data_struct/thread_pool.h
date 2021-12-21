#ifndef P2P_FILE_SYNC_PROTOCOL_UTILS_THREAD_POOL_H
#define P2P_FILE_SYNC_PROTOCOL_UTILS_THREAD_POOL_H

#include <glog/logging.h>

#include <algorithm>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <thread>
#include <utility>
#include <vector>

#include "cas_queue.h"

namespace P2PFileSync {
class ThreadPool {
 public:
  /**
   * @brief Construct a new Thread Pool
   * @note the max count of thread worker is the max size of uint8_t;
   * @param thread_size the thread worker size
   */
  explicit ThreadPool(const uint8_t thread_size)
      : _worker_pool(std::vector<std::thread>(thread_size)) {
    // initialize working thread
    for (size_t i = 0; i < _worker_pool.size(); ++i) {
      _worker_pool[i] = std::thread(ThreadWorker(this, i));
      VLOG(3) << "Thread " << i << " is created.";
    }
  };

  /**
   * @brief Destroy the Thread Pool, this function will do following things:
   *  1. Trying to wait every worker stop running ant delete the worker
   *
   */
  ~ThreadPool() {
    _running_flag = false;
    m_conditional_lock.notify_all();

    for (auto &i : _worker_pool) {
      if (i.joinable()) {
        i.join();
      }
    }
  };

  [[nodiscard]] size_t size() const { return _worker_pool.size(); }

  /**
   *
   * @tparam T
   * @param task
   * @return
   */
  template <typename T>
  std::future<T> submit(std::shared_ptr<std::packaged_task<T()>> task) {
    {
      std::unique_lock<std::shared_mutex> lock(m_mutex);
      _task_queue.push(std::move([task]() { (*task)(); }));
    }

    m_conditional_lock.notify_one();

    return task->get_future();
  }

  /**
   * @brief
   *
   * @tparam F
   * @tparam Args
   * @param f
   * @param args arguments form
   * @return std::future<decltype(f(args...))> future object as returned value
   */
  template <typename F, typename... Args>
  auto submit(F &&f, Args &&...args)
      -> std::future<typename std::decay<decltype(f(args...))>::type> {
    using return_type = typename std::decay<decltype(f(args...))>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    return submit<return_type>(task);
  }

  // prevent reference and copy
  ThreadPool(ThreadPool &&) = delete;
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(ThreadPool &&) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;

 private:
  std::shared_mutex m_mutex;
  bool _running_flag = true;
  std::vector<std::thread> _worker_pool;
  std::queue<std::function<void()>> _task_queue;

  // lock staff
  std::mutex _mutex;
  std::condition_variable m_conditional_lock;

  class ThreadWorker {
   public:
    /**
     * @brief override () operator for fetching task from server and execute the
     * task
     *
     */
    void operator()() {
      while (_pool->_running_flag) {
        {
          std::unique_lock<std::mutex> lock(_pool->_mutex);
          if (_pool->_task_queue.empty()) {
            VLOG(3) << "Thread " << std::this_thread::get_id() << " is waiting for task.";
            _pool->m_conditional_lock.wait(lock);
            VLOG(3) << "Thread " << std::this_thread::get_id() << " is waken up.";
          }
        }

        {
          std::shared_lock<std::shared_mutex> lock(_pool->m_mutex);

          if (!_pool->_task_queue.empty()) {
            auto task = std::move(_pool->_task_queue.front());
            _pool->_task_queue.pop();
            lock.unlock();
            task();
            VLOG(3) << "Thread " << std::this_thread::get_id() << " is executing task.";
          }
        }
      }
    };

   protected:
    /**
     * @brief Constructor for Thread worker, for prevent outer class
     * initialized ThreadWorker with unexpected behaviors, the ThreadWorker are
     * marked as protected and mark ThreadPool class as friend class which allow
     * ThreadPool called constructor
     *
     */
    friend ThreadPool;
    ThreadWorker(ThreadPool *pool, const uint8_t id) : _pool(pool), _id(id){};

   private:
    uint8_t _id;        // thread id
    ThreadPool *_pool;  // master pool reference
  };
};
}  // namespace P2PFileSync

#endif  // P2P_FILE_SYNC_PROTOCOL_UTILS_THREAD_POOL_H