#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <stdexcept>
#include <algorithm>

template <typename T>
struct Node{
    T value;
    Node* left;
    Node* right;
    int height;
    Node(const T& v) : value(v), left(nullptr), right(nullptr), height(1){}
};

template <class T>
class BinarySearchTree{
protected:
    Node<T>* root;
    int count;

    int Height(Node<T>* n) const{
        return n? n->height : 0;
    }
    void Update(Node<T>* n){
        n->height = 1 + std::max(Height(n->left), Height(n->right));
    }
    int Balance(Node<T>* n) const{
        return Height(n->left) - Height(n->right);
    }

    virtual Node<T>* Rebalance(Node<T>* n) { 
        Update(n); return n; 
    }

    Node<T>* InsertNode(Node<T>* node, const T& x){
        if (!node){
            return new Node<T>(x);
        }
        if (x < node->value){
            node->left = InsertNode(node->left, x);
        }
        else if(node->value < x){
            node->right = InsertNode(node->right, x);
        }
        else{
            throw std::invalid_argument("KeyAlreadyExists");
        }
        return Rebalance(node);
    }

    template <typename F>
    void InOrder(Node<T>* n, F f) const {
        if (n) {
            InOrder(n->left, f);     // спуститься: передаём УЗЕЛ-ребёнка
            f(n->value);             // обработать: передаём ЗНАЧЕНИЕ
            InOrder(n->right, f);    // спуститься: передаём УЗЕЛ-ребёнка
        }
    }
    
    void Clear(Node<T>* n){
        if (n){
            Clear(n->left);
            Clear(n->right);
            delete n;
        }
    }

public:
    BinarySearchTree() : root(nullptr), count(0){}
    virtual ~BinarySearchTree(){Clear(root);}

    BinarySearchTree(const BinarySearchTree&)=delete;
    BinarySearchTree& operator=(const BinarySearchTree&)=delete;

    void Add(const T& x){
        root = InsertNode(root, x);
        count++;
    }

    const T* Find(const T& x) const{
        Node<T>* cur = root;
        while(cur){
            if (x < cur->value){
                cur = cur->left;
            }
            else if (cur->value < x){
                cur = cur->right;
            }
            else{
                return &cur->value;
            }
        }
        return nullptr;
    }

    bool Contains(const T& x) const{
        return Find(x) != nullptr;
    }

    template <typename F>
    void ForEach(F f) const{
        InOrder(root, f);
    } 



    int GetCount() const{
        return count;
    }
    int GetHeight() const{
        return Height(root);
    }

};

#endif