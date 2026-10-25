#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "binarySearchTree.h"

template <typename T> class AVLTree : public BinarySearchTree<T>{
protected:
    Node<T>* RotateRight(Node<T>* y){
        Node<T>* x = y->left;
        Node<T>* B = x->right;
        x->right = y;
        y->left = B;
        this->Update(y);
        this->Update(x);
        return x;
    }

    Node<T>* RotateLeft(Node<T>* y){
        Node<T>* x = y->right;
        Node<T>* B = x->left;
        x->left = y;
        y->right = B;
        this->Update(y);
        this->Update(x);
        return x;
    }

    Node<T>* Rebalance(Node<T>* n) override{
        this->Update(n);
        int b = this->Balance(n);

        if (1 < b){
            if (this->Balance(n->left) < 0){
                n->left = RotateLeft(n->left);
            }
            return RotateRight(n);

        }
        if (b < -1){
            if (this->Balance(n->right) > 0){
                n->right = RotateRight(n->right);
            }
            return RotateLeft(n);

        }

        return n;
    }


};

#endif