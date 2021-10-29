#ifndef P2P_FILE_SYNC_PROTOCOL_UTILS_CAS_QUEUE_H
#define P2P_FILE_SYNC_PROTOCOL_UTILS_CAS_QUEUE_H
#include <atomic>
#include <memory>
#include <utility>
namespace P2PFileSync::Protocol {
/**
 * @brief A Lock-free Queue implement by CAS and Linked-List
 *  The Algorithm and implementing Idea are genearted by following paper:
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
  CASQueue() : _head(Node(nullptr)), _tail(_head){};

  /**
   * @brief Push data to the queue by CAS
   *
   * @param data the data
   */
  void push(T data) {
    std::atomic<Node*> node = new Node(data);

    while (true) {
      auto tail = _tail;
      auto next = _tail->get_next();
      if (tail == _tail) {
        if (next->get_data() == nullptr) {
          if (_tail->get_next().compare_exchange_strong(next, node)) {
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
      auto head = _head;
      auto tail = _tail;
      auto next = head->get_next();

      if (head == _head) {
        if (head == tail) {
          if (head->get_data() == nullptr) {
            return nullptr;
          }
          _tail.compare_exchange_strong(tail, next);
        } else {
          auto data = next->get_data();
          if (head.compare_exchange_strong(head, next)) {
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
     * @brief Construct a new Inner Node
     *
     * @param data the data wants to fill in the node
     */
    Node(T&& data) : _data(std::make_shared<T>(std::forward<T>(data))){};

    /**
     * @brief Get the data of this node
     *
     * @return T
     */
    T get_data() { return _data; }

    /**
     * @brief Setting the next object
     *
     * @param next the next node wants to setup
     */
    void set_next(std::atomic<Node*> next) { _next = next; };

    /**
     * @brief Get the next node
     *  This complecate implementation is getting ideas from
     * this(http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.53.8674&rep=rep1&type=pdf)
     * paper. Is used to solve ABA problem
     * @return std::atomic<Node*> the pointer of the next node
     */
    auto get_next() {
      auto ret = _next;

      while (true) {
        if (ret == nullptr) {
          return ret;
        }

        ref_count.fetch_add(1);

        if (ret == _next) {
          return ret;
        } else {
          ref_count.fetch_sub(1);
        }
      }
      return _next;
    };

   private:
    std::shared_ptr<T> _data;            // data container
    std::atomic<int> ref_count{0};       // refernce count
    std::atomic<Node*> _next = nullptr;  // next node refernce
  };

 private:
  std::atomic<Node*> _head;  // head of queue
  std::atomic<Node*> _tail;  // tail of queue
};
}  // namespace P2PFileSync::Protocol
#endif // P2P_FILE_SYNC_PROTOCOL_UTILS_CAS_QUEUE_H