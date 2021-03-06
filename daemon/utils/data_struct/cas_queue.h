#ifndef P2P_FILE_SYNC_PROTOCOL_UTILS_CAS_QUEUE_H
#define P2P_FILE_SYNC_PROTOCOL_UTILS_CAS_QUEUE_H
#include <atomic>
#include <memory>
#include <utility>
namespace P2PFileSync {
/**
 * @brief A Lock-free Queue implement by CAS and Linked-List
 *  The Algorithm and implementing Idea are generated by following paper:
 *    1. https://www.cs.rochester.edu/u/scott/papers/1996_PODC_queues.pdf
 *    2. http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.53.8674&rep=rep1&type=pdf
 *
 */
template <typename T>
class CASQueue {
 public:
  /**
   * @brief Construct a new CASQueue object which will initialized head and tail
   *
   */
  CASQueue()= default;;

  /**
   * @brief Push data to the queue by CAS
   *
   * @param data the data
   */
  void push(T&& data) {
    Node* node = new Node(std::forward<T&>(data));

    while (true) {
      auto tail = _tail.load();
      auto next = _tail.load()->next().load();
      if (tail == _tail.load()) {
        if (next == nullptr) {
          if (_tail.load()->next().compare_exchange_strong(next, node)) {
            break;
          }
        } else {
          _tail.compare_exchange_strong(tail, next);
        }
      }
      _tail.compare_exchange_strong(tail, node);
    }
  }

  /**
   * @brief pop data from array by CAS operation, pop will return nullptr if
   * there the queue is empty
   *
   * @return T the returned data
   */
  T pop() {
    while (true) {
      auto &head = _head;
      auto tail = _tail.load();
      auto next = head.load()->next().load();

      if (head == _head) {
        if (head == tail) {
          if (head == nullptr) {
            return nullptr;
          }
          _tail.compare_exchange_strong(tail, next);
        } else {
          auto data = next->data();
          auto head_node = head.load();
          if (head.compare_exchange_strong(head_node, next)) {
            delete next;
            return data;
          }
        }
      }
    }
  }

  /**
   * @brief Indicated the queue is empty or not
   *
   * @return true the queue is empty
   * @return false the queue is not empty
   */
  bool empty(){
    return _head == _tail;
  }
 protected:
  class Node {
   public:
    /**
     * @brief delete default empty constructor
     */
    Node() = delete;

    /**
     * @brief construct node by transferring ownership from other unique_ptr
     * @param args
     */
    explicit Node(T& object_ptr) noexcept : _data(std::move(object_ptr)){};

    /**
     * @brief Get the data of this node
     *
     * @return T
     */
    T& data() { return _data; }

    /**
     * @brief Get the next node
     *  This complicate implementation is getting ideas from
     * this(http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.53.8674&rep=rep1&type=pdf)
     * paper. Is used to solve ABA problem
     * @return std::atomic<Node*> the pointer of the next node
     */
    std::atomic<Node*>& next() {
      auto &ret = _next;

      while (true) {
        if (ret == nullptr) {
          return ret;
        }

        _ref.fetch_add(1);

        if (ret == _next) {
          return ret;
        } else {
          _ref.fetch_sub(1);
        }
      }
    };

   private:
    T _data;         // data container
    std::atomic<int> _ref {0};                 // reference count
    std::atomic<Node*> _next{nullptr};    // next node reference
  };

 private:
  std::atomic<Node*> _head{nullptr};  // head of queue
  std::atomic<Node*> &_tail = _head;  // ref of tail of queue
};
}  // namespace P2PFileSync::Protocol
#endif // P2P_FILE_SYNC_PROTOCOL_UTILS_CAS_QUEUE_H