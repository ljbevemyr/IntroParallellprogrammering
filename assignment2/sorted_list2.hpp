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
  std::mutex mutex_node;
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
  std::mutex mutex_list;
  /* insert v into the list */
  void insert(T v) {
    /* first find position */
    mutex_list.lock();
    if (first == nullptr) {
      first= new node<T>();
      first->value = v;
      mutex_list.unlock();
    }
    else if (first->value >= v) {
      node<T>* succ = first;
      first = new node<T>();
      first->value = v;
      first->next = succ;
      mutex_list.unlock();
    }
    else {
      node<T>* pred = first;
      node<T>* succ = pred->next;
      pred->mutex_node.lock();
      mutex_list.unlock();

      while(succ != nullptr && succ->value < v) {
        succ->mutex_node.lock();
        pred->mutex_node.unlock();
        pred = succ;
        succ = succ->next;
      }

      /* construct new node */
      node<T>* current = new node<T>();
      current->value = v;

      /* insert new node between pred and succ */
      current->next = succ;
      pred->next = current;

      pred->mutex_node.unlock();
    }
  }

  void remove(T v) {
    /* first find position */
    mutex_list.lock();
    node<T>* current = first;
    if (current == nullptr) {
      mutex_list.unlock();
    }
    else if (current->value == v) {
      first = first->next;
      delete current;
      mutex_list.unlock();
    }
    else {
      node<T>* pred = current;
      current = pred->next;
      pred->mutex_node.lock();
      mutex_list.unlock();

      while(current != nullptr && current->value < v) {
        current->mutex_node.lock();
        if (current->value == v)  {
          break;
        }
        pred->mutex_node.unlock();
        pred = current;
        current = current->next;
      }
      if(current == nullptr || current->value != v) {
        /* v not found */
        pred->mutex_node.unlock();
        return;
      }
      /* remove current */
      pred->next = current->next;
      delete current;
      pred->mutex_node.unlock();
    }
  }

  /* count elements with value v in the list */
  std::size_t count(T v) {
    std::size_t cnt = 0;
    /* first go to value v */
    mutex_list.lock();
    node<T>* current = first;
    current->mutex_node.lock();
    mutex_list.unlock();
    node<T>* pred = current;
    current = pred->next;
    while(current != nullptr && current->value < v) {
      current->mutex_node.lock();
      pred->mutex_node.unlock();
      pred = current;
      current = current->next;
    }
    /* count elements */
    while(current != nullptr && current->value == v) {
      cnt++;
      current->mutex_node.lock();
      pred->mutex_node.unlock();
      pred = current;
      current = current->next;
    }
    pred->mutex_node.unlock();
    return cnt;
  }
};

#endif // lacpp_sorted_list_hpp
