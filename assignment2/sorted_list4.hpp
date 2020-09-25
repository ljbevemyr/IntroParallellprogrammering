#ifndef lacpp_sorted_list_hpp
#define lacpp_sorted_list_hpp lacpp_sorted_list_hpp

/* a sorted list implementation by David Klaftenegger, 2015
 * please report bugs or suggest improvements to david.klaftenegger@it.uu.se
 */

/* struct for list nodes */
template<typename T>
struct node {
  T value;
  node<T>* next;
  std::atomic<bool> lock_node = ATOMIC_FLAG_INIT;
};

/* non-concurrent sorted singly-linked list */
template<typename T>
class sorted_list {
  node<T>* first = nullptr;

public:
  /* default implementations:
   * default constructor
   * copy constructor (note: shallow copy)
   * move constructor
   * copy assignment operator (note: shallow copy)
   * move assignment operator
   *
   * The first is required due to the others,
   * which are explicitly listed due to the rule of five.
   */
  sorted_list() = default;
  sorted_list(const sorted_list<T>& other) = default;
  sorted_list(sorted_list<T>&& other) = default;
  sorted_list<T>& operator=(const sorted_list<T>& other) = default;
  sorted_list<T>& operator=(sorted_list<T>&& other) = default;
  ~sorted_list() {
    while(first != nullptr) {
      remove(first->value);
    }
  }
  std::atomic<bool> lock_list = ATOMIC_FLAG_INIT;

  void TATASlock(std::atomic<bool> *lock) {
    do {
      while (*lock) {
        continue;
      }
    } while (lock->exchange(true)); // actual atomic locking
    return;
  }

  void TATASunlock(std::atomic<bool> *lock) {
    lock->exchange(false); //istället för exchange kanske det borde stå store
  }

  /* insert v into the list */
  void insert(T v) {
    /* first find position */
    TATASlock(&lock_list);
    if (first == nullptr) {
      first= new node<T>();
      first->value = v;
      TATASunlock(&lock_list);
    }
    else if (first->value >= v) {
      node<T>* succ = first;
      first = new node<T>();
      first->value = v;
      first->next = succ;
      TATASunlock(&lock_list);
    }
    else {
      node<T>* pred = first;
      node<T>* succ = pred->next;
      TATASlock(&pred->lock_node);
      TATASunlock(&lock_list);

      while(succ != nullptr && succ->value < v) {
        TATASlock(&succ->lock_node);
        TATASunlock(&pred->lock_node);
        pred = succ;
        succ = succ->next;
      }

      /* construct new node */
      node<T>* current = new node<T>();
      current->value = v;

      /* insert new node between pred and succ */
      current->next = succ;
      pred->next = current;

      TATASunlock(&pred->lock_node);
    }
  }

  void remove(T v) {
    /* first find position */
    TATASlock(&lock_list);
    node<T>* current = first;
    if (current == nullptr) {
      TATASunlock(&lock_list);
    }
    else if (current->value == v) {
      first = first->next;
      delete current;
      TATASunlock(&lock_list);
    }
    else {
      node<T>* pred = current;
      current = pred->next;
      TATASlock(&pred->lock_node);
      TATASunlock(&lock_list);

      while(current != nullptr && current->value < v) {
        TATASlock(&current->lock_node);
        if (current->value == v)  {
          break;
        }
        TATASunlock(&pred->lock_node);
        pred = current;
        current = current->next;
      }
      if(current == nullptr || current->value != v) {
        /* v not found */
        TATASunlock(&pred->lock_node);
        return;
      }
      /* remove current */
      pred->next = current->next;
      delete current;
      TATASunlock(&pred->lock_node);
    }
  }

  /* count elements with value v in the list */
  std::size_t count(T v) {
    std::size_t cnt = 0;
    /* first go to value v */
    TATASlock(&lock_list);
    node<T>* current = first;
    TATASlock(&current->lock_node);
    TATASunlock(&lock_list);
    node<T>* pred = current;
    current = pred->next;
    while(current != nullptr && current->value < v) {
      TATASlock(&current->lock_node);
      TATASunlock(&pred->lock_node);
      pred = current;
      current = current->next;
    }
    /* count elements */
    while(current != nullptr && current->value == v) {
      cnt++;
      TATASlock(&current->lock_node);
      TATASunlock(&pred->lock_node);
      pred = current;
      current = current->next;
    }
    TATASunlock(&pred->lock_node);
    return cnt;
  }
};

#endif // lacpp_sorted_list_hpp
