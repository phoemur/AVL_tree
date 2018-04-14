#ifndef AVL_MAP_HEADER_HPP
#define AVL_MAP_HEADER_HPP

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace Homebrew {  
    
template<typename Key, typename Value>
class AvlMap {
    
    // Node of the tree and proxy class for return values
    struct Node {
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        Key key;
        Value value;
        std::int32_t height;
        
        template<typename K = Key,
                 typename V = Value>
        Node(K&& k, 
             V&& v,
             std::unique_ptr<Node>&& lt,
             std::unique_ptr<Node>&& rt,
             std::int32_t h = 0)
                : left{std::move(lt)},
                  right{std::move(rt)},
                  key{std::forward<K>(k)},
                  value{std::forward<V>(v)},
                  height{h} {}
                  
        operator Value& () {return value;}
        operator const Value& () const {return value;}
        //Value& operator*() {return value;}
        //const Value& operator*() const {return value;}
    };
    
    // root of the tree
    std::unique_ptr<Node> root;
    
    // Number of elements
    std::size_t sz;
    
public:
    // constructors block
    AvlMap() : root{nullptr}, sz{0} {}
    
    AvlMap(const AvlMap& other)
        : root{clone(other.root)}, sz{other.sz} {}
        
    AvlMap& operator=(const AvlMap& other) 
    {
        // copy and swap idiom
        AvlMap tmp (other);
        std::swap(root, tmp.root);
        std::swap(sz, tmp.sz);

        return *this;
    }
    
    AvlMap(AvlMap&& other) noexcept
        : AvlMap()
    {
        std::swap(root, other.root);
        std::swap(sz, other.sz);
    }
    
    AvlMap& operator=(AvlMap&& other) noexcept
    {
        std::swap(root, other.root);
        std::swap(sz, other.sz);
        return *this;
    }
    
    ~AvlMap() = default;
    
    template<typename Iter>
    AvlMap(Iter first, Iter last)
        : AvlMap()
    {
        // Check std::pair type
        using k_tp = typename std::iterator_traits<Iter>::value_type::first_type;
        static_assert(std::is_constructible<Key, k_tp>::value, "Type mismatch");
        using v_tp = typename std::iterator_traits<Iter>::value_type::second_type;
        static_assert(std::is_constructible<Value, v_tp>::value, "Type mismatch");
        
        for (auto it = first; it != last; ++it) {
            insert(it->first, it->second);
        }
    }
    
    AvlMap(const std::initializer_list<std::pair<const Key, Value>>& lst)
        : AvlMap(std::begin(lst), std::end(lst)) {}

    
    AvlMap& operator=(const std::initializer_list<std::pair<const Key, Value>>& lst)
    {
        // copy and swap idiom
        AvlMap tmp (lst);
        std::swap(root, tmp.root);
        std::swap(sz, tmp.sz);

        return *this;
    }
    
    // Member functions block
    void clear() noexcept
    {
        root.reset(nullptr);
    }
    
    inline bool empty() const noexcept
    {
        return sz == 0;
    }
    
    inline const std::size_t& size() const noexcept
    {
        return sz;
    }
    
    template<typename K = Key,
             typename V = Value>
    void insert(K&& k, V&& v)
    {
        insert_util(std::forward<K>(k), std::forward<V>(v), root);
        ++sz;
    }
    
    void erase(const Key& k) noexcept
    {
        remove_util(k, root);
        --sz;
    }
    
    void print() const noexcept
    {
        if (empty()) std::cout << "{}\n";
        else {
            std::cout << "{";
            print(root);
            std::cout << "\b\b}\n";
        }
    }
    
    //access or insert specified element 
    Value& operator[](const Key& k)
    {
        std::unique_ptr<Node>& ptr = search(k, root);
        
        if (ptr == nullptr) {
            // insert default constructed value
            insert_util(k, Value(), ptr);
            ++sz;
        }
        
        return *ptr;
    }
    
    const Value& operator[](const Key& k) const noexcept
    {
        auto& ptr = search(k, root);
        return ptr != nullptr ? *ptr : Value();
    }
    
    // access specified elem with checking
    Value& at(const Key& k)
    {
        auto& ptr = search(k, root);
        if (ptr == nullptr) throw std::out_of_range("Elem not found error");
        return *ptr;
    }
    
    const Value& at(const Key& k) const
    {
        return at(k);
    }
    
    
    // Check if conatiner has an specific key
    bool search(const Key& x) const noexcept
    {
        return search(x, root) != nullptr;
    }    
        
private:
    // recursive method to clone a tree
    std::unique_ptr<Node> clone(const std::unique_ptr<Node>& node) const
    {
        if (!node) return nullptr;
        else
            return std::make_unique<Node>(node->key,
                                          node->value,
                                          clone(node->left), 
                                          clone(node->right), 
                                          node->height);
    }
    
    // Returns height of a node
    inline std::int32_t height(const std::unique_ptr<Node>& node) const noexcept
    {
        return node == nullptr ? -1 : node->height;
    }
    
    // print tree inorder
    void print(const std::unique_ptr<Node>& t) const noexcept
    { 
        if (t != nullptr) {
            
            print(t->left);
            std::cout << "(" << t->key << ", " << t->value << "), ";
            print(t->right);
        }
    }
    
    
    // binary search an element in the tree
    std::unique_ptr<Node>& search(const Key& x, std::unique_ptr<Node>& t) const noexcept
    {
        // recursive
        /*return (t == nullptr || t->key == x) ? t : 
                                                 x < t->key ? search(x, t->left) : 
                                                              search(x, t->right);
        
        */
        
        // iterative 
        if (t == nullptr || t->key == x) return t;
        
        Node* tmp = t.get();
        
        while(true) {
            if(x < tmp->key) {
                if (tmp->left == nullptr || tmp->left->key == x) {
                    return tmp->left;
                }
                tmp = tmp->left.get();
            }
            else if(tmp->key < x) {
                if (tmp->right == nullptr || tmp->right->key == x) {
                    return tmp->right;
                }
                tmp = tmp->right.get();
            }
        }
    }
    
    
    // Recursive insert method
    template<typename K = Key,
             typename V = Value>
    void insert_util(K&& k, V&& v, std::unique_ptr<Node>& t)
    {
        if (t == nullptr)
            t = std::make_unique<Node>(std::forward<K>(k), std::forward<V>(v), nullptr, nullptr, 0);
        else if (k < t->key)
            insert_util(std::forward<K>(k), std::forward<V>(v), t->left);
        else if (t->key < k)
            insert_util(std::forward<K>(k), std::forward<V>(v), t->right);
        
        balance(t);
    }
    
    // Recursive delete method
    void remove_util(const Key& x, std::unique_ptr<Node>& t) noexcept
    {
        if(t == nullptr) return;   // Item not found; do nothing
        
        if(x < t->key)
            remove_util(x, t->left);
        else if(t->key < x)
            remove_util(x, t->right);
        else if(t->left != nullptr && t->right != nullptr) { // Two children
            t->key = findMin(t->right)->key;
            remove_util(t->key, t->right);
        }
        else { // One child
            std::unique_ptr<Node> oldNode {std::move(t)};
            t = (oldNode->left != nullptr) ? std::move(oldNode->left) : 
                                             std::move(oldNode->right);
            
            // oldNode.reset(nullptr); -> unneeded, auto delete when go out of scope
        }
        
        balance(t);
    }
    
    // Find smallest elem in a tree
    Node* findMin(const std::unique_ptr<Node>& node) const noexcept
    {
        auto t = node.get();

        if(t != nullptr)
            while (t->left != nullptr)
                t = t->left.get();

        return t;
    }
    
    // Find largest elem in a tree
    Node* findMax(const std::unique_ptr<Node>& node) const noexcept
    {
        auto t = node.get();

        if(t != nullptr)
            while (t->right != nullptr)
                t = t->right.get();

        return t;
    }
    
    // Internal method to re-balance the tree
    void balance(std::unique_ptr<Node>& t) noexcept
    {
        static const int ALLOWED_IMBALANCE = 1;
        
        if(t == nullptr) return;
        
        if(height(t->left) - height(t->right) > ALLOWED_IMBALANCE) {
            if(height(t->left->left) >= height(t->left->right))
                rotateWithLeftChild(t);
            else
                doubleWithLeftChild(t);
        }
        else if(height(t->right) - height(t->left) > ALLOWED_IMBALANCE) {
            if(height(t->right->right) >= height(t->right->left))
                rotateWithRightChild(t);
            else
                doubleWithRightChild(t);
        }
        
        t->height = std::max(height(t->left), height(t->right)) + 1;
    }
    
    // Rotations block
    
    /**
     * Rotate binary tree node with left child.
     * For AVL trees, this is a single rotation for case 1.
     * Update heights, then set new root.
     */
    void rotateWithLeftChild(std::unique_ptr<Node>& k2) noexcept
    {
        auto k1 = std::move(k2->left);
        k2->left = std::move(k1->right);
        k2->height = std::max(height(k2->left), height(k2->right)) + 1;
        k1->height = std::max(height(k1->left), k2->height) + 1;
        k1->right = std::move(k2);
        k2 = std::move(k1);
    }
    
    /**
     * Rotate binary tree node with right child.
     * For AVL trees, this is a single rotation for case 4.
     * Update heights, then set new root.
     */
    void rotateWithRightChild(std::unique_ptr<Node>& k1) noexcept
    {
        auto k2 = std::move(k1->right);
        k1->right = std::move(k2->left);
        k1->height = std::max(height(k1->left), height(k1->right)) + 1;
        k2->height = std::max(height(k2->right), k1->height) + 1;
        k2->left = std::move(k1);
        k1 = std::move(k2);
    }
    
    /**
     * Double rotate binary tree node: first left child.
     * with its right child; then node k3 with new left child.
     * For AVL trees, this is a double rotation for case 2.
     * Update heights, then set new root.
     */
    void doubleWithLeftChild(std::unique_ptr<Node>& k3) noexcept
    {
        rotateWithRightChild(k3->left);
        rotateWithLeftChild(k3);
    }

    /**
     * Double rotate binary tree node: first right child.
     * with its left child; then node k1 with new right child.
     * For AVL trees, this is a double rotation for case 3.
     * Update heights, then set new root.
     */
    void doubleWithRightChild(std::unique_ptr<Node>& k1) noexcept
    {
        rotateWithLeftChild(k1->right);
        rotateWithRightChild(k1);
    }
    
}; // end of class AvlMap
    
} // ned of namespace Homebrew

#endif // AVL_MAP_HEADER_HPP