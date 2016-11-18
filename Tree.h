//
// Created by sm4ll_3gg on 12.11.16.
//

#ifndef TREE_TREE_H
#define TREE_TREE_H

#include "TException.h"

#include <iostream>
#include <functional>

template <typename DataType>
class Tree
{
public:
    struct Node
    {
        DataType data;

        Node* right;
        Node* left;

        Node* parent;
    };

    Node* root;

    void                    free_resources(Node*) noexcept;

    Node*                   _search(Node*, const DataType&) noexcept;

    void                    transplant(Node*,Node*) noexcept;

    Node*                   minimum_node(Node*) noexcept; // В поддереве
    Node*                   maximum_node(Node*) noexcept; // -----------

    Node*                   find_place_to_add(const DataType&) noexcept;

    void                    _clr_tree_walk(Node*, std::function<void (Node*)>) noexcept;
    void                    _lcr_tree_walk(Node*, std::function<void (Node*)>) noexcept;
    void                    _lrc_tree_walk(Node*, std::function<void (Node*)>) noexcept;

    int                     _depth(Node*, const int&) noexcept;

    void                    _print_tree(Node*, int) noexcept;
public:
    Tree() noexcept;
    Tree(const DataType&) throw( TException );
    Tree(Node*) noexcept ; // Создание поддерева с заданного узла
    Tree(const Tree<DataType>&) noexcept ;
    ~Tree() noexcept;

    void                    push(const DataType&) throw( TException );
    void                    clear() noexcept;

    void                    remove(const DataType&) noexcept;
    void                    cut(const DataType&) noexcept;

    Node*                   search(const DataType&) throw( TException );

    const DataType&         min() noexcept;
    const DataType&         max() noexcept;

    void                    add_subtree(const Tree<DataType>&) noexcept;
    Tree<DataType>          get_subtree(const DataType&) noexcept;

    void                    clr_tree_walk(std::function<void (Node*)>) noexcept;
    void                    lcr_tree_walk(std::function<void (Node*)>) noexcept;

    int                     depth() noexcept; // Криво считает глубину, после добавления поддерева

    const Tree<DataType>&   operator = (const Tree<DataType>&) noexcept ;

    void                    print_tree() noexcept;
};

template <typename DataType>
Tree<DataType>::Tree() noexcept
{
    root = nullptr;
}

template <typename DataType>
Tree<DataType>::Tree(const DataType& data) throw( TException )
{
    try
    {
        root = new Node;

        root->data = data;

        root->right = nullptr;
        root->left = nullptr;

        root->parent = nullptr;
    }
    catch(std::bad_alloc& ba)
    {
        throw TException("Bad Alloc!");
    }
}

template <typename DataType>
Tree<DataType>::Tree(Node* new_root) noexcept
{
    root = nullptr;

    auto func = [this] (Node* node)
    {
        push(node->data);
    };

    _clr_tree_walk(new_root, func);
}

template <typename DataType>
Tree<DataType>::Tree(const Tree<DataType>& other) noexcept
{
    root = nullptr;

    auto func = [this] (Node* node)
    {
        push(node->data);
    };

    _clr_tree_walk(other.root,func);
}

template <typename DataType>
Tree<DataType>::~Tree() noexcept
{
    if(root != nullptr)
    {
        free_resources(root);
    }
}

template <typename DataType>
void Tree<DataType>::push(const DataType& data) throw( TException )
{
    try
    {
        Node* node = new Node; // вставляемый узел

        node->data = data;
        node->left = nullptr;
        node->right = nullptr;
        node->parent = nullptr;

        Node* temp = nullptr;
        Node* y = root;

        while(y != nullptr)
        {
            temp = y;

            if(data < y->data)
            {
                y = y->left;
            }
            else
            {
                y = y->right;
            }
        }

        node->parent = temp;

        if(temp == nullptr)
        {
            root = node;
        }
        else
        {
            if(data < temp->data)
            {
                temp->left = node;
            }
            else
            {
                temp->right = node;
            }
        }


    }
    catch(std::bad_alloc& ba)
    {
        throw TException("Bad Alloc!");
    }


}

template <typename  DataType>
void Tree<DataType>::clear() noexcept
{
    free_resources(root);

    root = nullptr;
}

template <typename DataType>
void Tree<DataType>::remove(const DataType& data) noexcept
{
    Node* node = search(data);

    free_resources(node);
}

template <typename DataType>
void Tree<DataType>::cut(const DataType& data) noexcept
{
    Node* node = search(data);

    if(node->left == nullptr)
    {
        transplant(node,node->right);
    }
    else
    if(node->right == nullptr)
    {
        transplant(node,node->left);
    }
    else
    {
        Node* temp = minimum_node(node->right);
        if(temp->parent != node)
        {
            transplant(temp,temp->right);
            temp->right = node->right;
            temp->right->parent = temp;
        }
        transplant(node,temp);
        temp->left = node->left;
        temp->left->parent = temp;
    }
}

template <typename DataType>
typename Tree<DataType>::Node* Tree<DataType>::search(const DataType& data) throw( TException )
{
    Node* node = root;

    while(node != nullptr)
    {
        if(data == node->data)
        {
            return node;
        }
        else if(data < node->data)
        {
            node = node->left;
        }
        else
        {
            node = node->right;
        }
    }

    throw TException("Узла с таким значением в данном дереве нет!");
}

template <typename DataType>
const DataType& Tree<DataType>::min() noexcept
{
    return minimum_node(root)->data;
}

template <typename DataType>
const DataType& Tree<DataType>::max() noexcept
{
    return maximum_node(root)->data;
}

template <typename DataType>
void Tree<DataType>::add_subtree(const Tree<DataType>& subtree) noexcept
{
    Node* temp = find_place_to_add(subtree.root->data);

    Tree<DataType>* temp_tree = new Tree<DataType>;
    *temp_tree = subtree;

    if(temp_tree->root->data < temp->data)
    {
        temp->left = temp_tree->root;
        temp_tree->root->parent = temp->left;
    }
    else
    {
        temp->right = temp_tree->root;
        temp_tree->root->parent = temp->right;
    }

    temp_tree = nullptr;
}

template <typename DataType>
Tree<DataType> Tree<DataType>::get_subtree(const DataType& data) noexcept
{
    Node* node = search(data);

    return Tree<DataType>(node);
}

template <typename DataType>
void Tree<DataType>::clr_tree_walk(std::function<void(Node*)> function) noexcept
{
    _clr_tree_walk(root, function);
}

template <typename DataType>
void Tree<DataType>::lcr_tree_walk(std::function<void(Node*)> function) noexcept
{
    _lcr_tree_walk(root, function);
}

template <typename DataType>
int Tree<DataType>::depth() noexcept
{
    return _depth(root,1);
}

template <typename DataType>
const Tree<DataType>& Tree<DataType>::operator=(const Tree<DataType>& other) noexcept
{
    if(root != nullptr)
    {
        free_resources(root);
    }

    auto func = [this](Node *node)
    {
        push(node->data);
    };

    _clr_tree_walk(other.root, func);

    return *this;
}

template <typename DataType>
void Tree<DataType>::print_tree() noexcept
{
    if(root == nullptr)
    {
        std::cout << "Дерево пустое" << std::endl;
    }
    else
    {
        _print_tree(root, 0);
    }
}

//
//---------------------Private Methods---------------------
//


template <typename DataType>
void Tree<DataType>::free_resources(Node* node) noexcept
{
    auto func = [] (Node* node)
    {
        /*
         * Если есть указатели на детей, то они
         * обнуляться из детей след. if'ом
         */

        if(node->parent != nullptr)
        {
            if (node->data < node->parent->data)
            {
                node->parent->left = nullptr;
            }
            else
            {
                node->parent->right = nullptr;
            }
        }

        node->data = DataType();
        node->parent = nullptr;
        delete node;

        node = nullptr;
    };

    _lrc_tree_walk(node,func);
}

template <typename DataType>
typename Tree<DataType>::Node* Tree<DataType>::_search(Node* node, const DataType& data) noexcept
{

//    if(node->data == data)
//    {
//        return node;
//    }
//    else
//    {
//        if(data < node->data && node->left != nullptr)
//        {
//            _search(node->left, data);
//        }
//        else if(node->right != nullptr)
//        {
//            _search(node->left, data);
//        }
//    }
}

template<typename DataType>
void Tree<DataType>::transplant(Node* u,Node* v) noexcept
{
    if(u->parent == nullptr)
    {
        root = v;
    }
    else
    if(u == u->parent->left)
    {
        u->parent->left = v;
    }
    else
    {
        u->parent->right = v;
    }

    if(v != nullptr)
    {
        v->parent = u->parent;
    }
}

template <typename DataType>
typename Tree<DataType>::Node* Tree<DataType>::minimum_node(Node* node) noexcept
{
    Node* temp = node;

    while(temp->left != nullptr)
    {
        temp = temp->left;
    }

    return temp;
}

template <typename DataType>
typename Tree<DataType>::Node* Tree<DataType>::maximum_node(Node* node) noexcept
{
    Node* temp = node;

    while(temp->right != nullptr)
    {
        temp = temp->right;
    }

    return temp;
}

template <typename DataType>
typename Tree<DataType>::Node* Tree<DataType>::find_place_to_add(const DataType& data) noexcept
{
    Node* node = root;

    while(node->left != nullptr || node->right != nullptr)
    {
        if(node->left != nullptr && data < node->data)
        {
            node = node->left;
        }
        else if(node->right != nullptr)
        {
            node = node->right;
        }
        else
        {
            break;
        }
    }

    return node;
}

template <typename DataType>
void Tree<DataType>::_clr_tree_walk(Node* node, std::function<void (Node*)> function) noexcept
{
    function(node);

    if(node->left != nullptr)
        _clr_tree_walk(node->left, function);

    if(node->right != nullptr)
        _clr_tree_walk(node->right, function);
}

template <typename DataType>
void Tree<DataType>::_lcr_tree_walk(Node* node, std::function<void (Node*)> function) noexcept
{
    if(node->left != nullptr)
        _lcr_tree_walk(node->left, function);

    function(node);

    if(node->right != nullptr)
        _lcr_tree_walk(node->right, function);
}

template <typename DataType>
void Tree<DataType>::_lrc_tree_walk(Node* node, std::function<void (Node*)> function) noexcept
{
    if(node->left != nullptr)
        _lrc_tree_walk(node->left, function);

    if(node->right != nullptr)
        _lrc_tree_walk(node->right, function);

    function(node);
}

template <typename DataType>
int Tree<DataType>::_depth(Node* node, const int& level) noexcept
{
    int left = 0;
    int right = 0;

    if(node->left != nullptr)
        left = _depth(node->left, level+1);
    if(node->right != nullptr)
        right = _depth(node->right, level+1);

    if(left > level)
    {
        return left;
    }
    else if(right > level)
    {
        return right;
    }
    else
        return level;
}

template <typename DataType>
void Tree<DataType>::_print_tree(Node* node, int level) noexcept
{
    for(int i = 0; i < level; i++)
    {
        std::cout << "  ";
    }
    std::cout << node->data << std::endl;

    if(node->left != nullptr) _print_tree(node->left, level+1);
    if(node->right != nullptr) _print_tree(node->right, level+1);
}

#endif //TREE_TREE_H
