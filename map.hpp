#ifndef MAP_HPP_
#define MAP_HPP_

#include <utility>

namespace dsc {
template <typename K, typename V>
class Map {
    public:
    // ctors and dtor
    Map() = default; // default constructor
    Map(const Map& original); // copy constructor
    Map(Map&& original); // move constructor
    Map(std::initializer_list<std::pair<const K, const V> l); // initializer list constructor
    ~Map(); // destructor

    // overloads
    Map& operator=(const Map& original); // copy assignment
    Map& operator=(Map&& original); // move assignment
    V& operator[](const K& key); // access operator

    // member functions
    void add(const std::pair<K, V>(const K& key, const V& value)); // Incorrect syntax?
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
    void add_impl(Node* current, const K& key);
    void remove_impl(Node* current, const K& key);

    Node** ptr_to(Node* current); // necessary?
    Node* prune(Node* current);
    std::size_t size_impl(Node* root) const;
};
} // namespace


#endif