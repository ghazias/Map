#ifndef MAP_HPP_
#define MAP_HPP_

#include <utility>

namespace dsc {
template <typename K, typename V>
class Map {
 public:
  // ctors and dtor
  Map() = default;           // default constructor
  Map(const Map& original);  // copy constructor
  Map(Map&& original);       // move constructor
  Map(std::initializer_list<std::pair<const K, const V> >
          list);  // initializer list constructor
  ~Map();         // destructor

  // overloads
  Map& operator=(const Map& original);  // copy assignment
  Map& operator=(Map&& original);       // move assignment
  V& operator[](const K& key);          // access operator

  // member functions
  void add(const std::pair<K, V> element);
  bool contains(const K& key) const;
  V& get(const K& key);
  // get() needs a const overload?
  void remove(const K& key);
  bool empty() const { return root_ == nullptr; }
  std::size_t size() const { return size_impl(root_); }

 private:
  struct Node {
    std::pair<const K, V> element;
    Node* parent;
    Node* left;
    Node* right;
  };

  Node* root_{};

  void destroy(Node* current);
  Node* copy(Node* parent, Node* current);
  Node* search(const K& key) const;
  Node* search_impl(Node* current, const K& key) const;
  void add_impl(Node* current, const std::pair<K, V> element);
  void remove_impl(Node* current, const K& key);

  Node* find_min(Node* current);
  Node** ptr_to(Node* current);
  Node* prune(Node* current);
  std::size_t size_impl(Node* root) const;
};
}  // namespace dsc

template <typename K, typename V>
dsc::Map<K, V>::Map(const Map& original)
    : root_{copy(nullptr, original.root_)} {}  // copy constructor

template <typename K, typename V>
auto dsc::Map<K, V>::copy(Node* parent, Node* current) -> Node* {
  if (!current) {
    return nullptr;
  }

  auto new_node = new Node{current->element, parent, nullptr, nullptr};
  new_node->left = copy(new_node, current->left);
  new_node->right = copy(new_node, current->right);
  return new_node;
}

template <typename K, typename V>
dsc::Map<K, V>::Map(Map<K, V>&& original) : root_{original.root_} {
  original.root_ = nullptr;
}  // move constructor

template <typename K, typename V>
dsc::Map<K, V>::Map(std::initializer_list<std::pair<const K, const V> > list) {
  for (const auto& p : list) {
    add(p);
  }
}  // initializer list constructor

template <typename K, typename V>
dsc::Map<K, V>::~Map() {
  destroy(root_);
  root_ = nullptr;
}  // destructor

template <typename K, typename V>
void dsc::Map<K, V>::destroy(Node* current) {
  if (current != nullptr) {
    destroy(current->left);
    destroy(current->right);
    delete current;
  }
}

template <typename K, typename V>
dsc::Map<K, V>& dsc::Map<K, V>::operator=(const Map<K, V>& original) {
  if (this != &original) {
    destroy(root_);
    root_ = copy(nullptr, original.root_);
  }

  return *this;
}  // copy assignment

template <typename K, typename V>
dsc::Map<K, V>& dsc::Map<K, V>::operator=(Map<K, V>&& original) {
  if (this != original) {
    destroy(root_);
    root_ = original.root_;
    original.root_ = nullptr;
  }

  return *this;
}  // move assignment

template <typename K, typename V>
V& dsc::Map<K, V>::operator[](const K& key) {
  return get(key);
}

template <typename K, typename V>
void dsc::Map<K, V>::add(const std::pair<K, V> element) {
  if (root_ == nullptr) {
    root_ = new Node { element, nullptr, nullptr, nullptr }
  } else {
    add_impl(root_, std::pair<K, V>{key, value});
  }
}

template <typename K, typename V>
void dsc::Map<K, V>::add_impl(Node* current, const std::pair<K, V> element) {
  if (element < current->element) {  // would be left child?
    if (current->left == nullptr) {
      current->left = new Node{element, current, nullptr, nullptr};
      return;
    } else {
      add_impl(current->left, element);  // recurse left
    }
  } else {  // would be right child?
    if (current->right == nullptr) {
      current->right = new Node{element, current, nullptr, nullptr};
      return;
    } else {
      add_impl(current->right, element);  // recurse right
    }
  }
}

template <typename K, typename V>
bool dsc::Map<K, V>::contains(const K& key) const {
  return search(element);
}

template <typename K, typename V>
V& dsc::Map<K, V>::get(const K& key) {
  return search(key)->element.second;
}

template <typename K, typename V>
void remove(const K& key) {
  remove_impl(root_, key);
}

template <typename K, typename V>
void dsc::Map<K, V>::remove_impl(Node* current, const K& key) {
  if (current == nullptr) {
    return;
  }
  if (element < current->element) {
    remove_impl(current->left, element);  // recurse left
  }
  if (element > current->element) {
    remove_impl(current->right, element);  // recurse right
  }

  if (current->element == element) {
    prune(current);
    delete current;
  }
}

template <typename K, typename V>
auto dsc::Map<K, V>::ptr_to(Node* current) -> Node** {
  if (current->parent == nullptr) {
    return &root_;
  }

  if (current->parent->left == current) {
    return &current->parent->left;
  }

  assert(current->parent->right == current);
  return &current->parent->right;
}

template <typename K, typename V>
auto dsc::Map<K, V>::find_min(Node* current) -> Node* {
  return current->left == nullptr ? current : find_min(current->left);
}

template <typename K, typename V>
auto dsc::Map<K, V>::prune(Node* current) -> Node* {
  if (current == nullptr) {
    return nullptr;
  }

  if (current->left == nullptr && current->right == nullptr) {
    *ptr_to(current) = nullptr;
    return current;
  }  // node is leaf

  if (current->left != nullptr && current->right == nullptr) {
    *ptr_to(current) = current->left;
    current->left->parent = current->parent;
    return current;
  }  // left child only

  if (current->right != nullptr && current->left == nullptr) {
    *ptr_to(current) = current->right;
    current->right->parent = current->parent;
    return current;
  }  // right child only

  assert(current->left != nullptr &&
         current->right != nullptr);           // two children
  Node* successor = find_min(current->right);  // find successor
  prune(successor);
  successor->parent = current->parent;  // update successor's pointers
  successor->left = current->left;
  successor->right = current->right;

  *ptr_to(current) = successor;  // update current->parent to point to successor
  current->left->parent =
      successor;  // update current children to point to successor
  if (current->right != nullptr) {
    // if current->right had no left children, then successor IS current->right
    // and was pruned so current doesn't have a right child
    current->right->parent = successor;
  }

  return current;
}

#endif
