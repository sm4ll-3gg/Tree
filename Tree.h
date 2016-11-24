//
// Created by sm4ll_3gg on 12.11.16.
//

#ifndef TREE_TREE_H
#define TREE_TREE_H

#include "TException.h"
#include "Node.h"

#include <iostream>
#include <functional>
#include <queue>
#include <forward_list>

template <typename DataType>
class Tree
{
    Node<DataType>* root;

public:
    Tree() noexcept;
    Tree(const DataType&) throw( TException );
    Tree(Node<DataType>*) noexcept ; // Создание поддерева с заданного узла
    Tree(const Tree<DataType>&) noexcept ;
    ~Tree() noexcept;

    void                                    push(const DataType&) throw( TException );
    void                                    clear() noexcept;

    void                                    remove(const DataType&) noexcept;
    void                                    cut(const DataType&) noexcept;

    Node<DataType>*                         search_first(const DataType&) throw( TException );
    std::forward_list< Node<DataType>* >    search_all(const DataType&) throw( TException );

    const DataType&                         min() noexcept;
    const DataType&                         max() noexcept;

    void                                    add_subtree(const Tree<DataType>&) noexcept;
    Tree<DataType>                          get_subtree(const DataType&) noexcept;

    void                                    clr_tree_traversial(std::function<void (Node<DataType>*)>) noexcept;
    void                                    lcr_tree_traversial(std::function<void (Node<DataType>*)>) noexcept;
    void                                    lrc_tree_traversial(std::function<void (Node<DataType>*)>) noexcept;
    void                                    breadth_first_traversal(std::function<void (Node<DataType>*)>) noexcept;

    int                                     depth() noexcept;

    const Tree<DataType>&                   operator = (const Tree<DataType>&) noexcept ;

    void                                    print_tree() noexcept;

private:
    void                                    free_resources(Node<DataType>*) noexcept;

    void                                    transplant(Node<DataType>*,Node<DataType>*) noexcept;

    Node<DataType>*                         minimum_node(Node<DataType>*) noexcept; // В поддереве
    Node<DataType>*                         maximum_node(Node<DataType>*) noexcept; // -----------

    Node<DataType>*                         find_place_to_add(const DataType&) noexcept;

    void                                    _clr_tree_traversial(Node<DataType>*, std::function<void (Node<DataType>*)>) noexcept;
    void                                    _lcr_tree_traversial(Node<DataType>*, std::function<void (Node<DataType>*)>) noexcept;
    void                                    _lrc_tree_traversial(Node<DataType>*, std::function<void (Node<DataType>*)>) noexcept;

    int                                     _depth(Node<DataType>*) noexcept;

    void                                    _print_tree(Node<DataType>*, int) noexcept;
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
        root = new Node<DataType>;

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
Tree<DataType>::Tree(Node<DataType>* new_root) noexcept
{
    root = nullptr;

    auto func = [this] (Node<DataType>* node)
    {
        push(node->data);
    };

    _clr_tree_traversial(new_root, func);
}

template <typename DataType>
Tree<DataType>::Tree(const Tree<DataType>& other) noexcept
{
    root = nullptr;

    auto func = [this] (Node<DataType>* node)
    {
        push(node->data);
    };

    _clr_tree_traversial(other.root,func);
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
        Node<DataType>* node = new Node<DataType>; // вставляемый узел

        node->data = data;
        node->left = nullptr;
        node->right = nullptr;
        node->parent = nullptr;

        Node<DataType>* temp = nullptr;
        Node<DataType>* y = root;

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
    std::forward_list< Node<DataType>* > nodes = search_all(data);

    for(auto it = nodes.begin(); it != nodes.end(); ++it)
    {
        free_resources(*it);
    }
}

template <typename DataType>
void Tree<DataType>::cut(const DataType& data) noexcept
{
    std::forward_list< Node<DataType>* > nodes = search_all(data);

    for(auto it = nodes.begin(); it != nodes.end(); ++it)
    {
        Node<DataType>* node = *it;

        if (node->left == nullptr)
        {
            transplant(node, node->right);
        }
        else if (node->right == nullptr)
        {
            transplant(node, node->left);
        }
        else
        {
            Node<DataType> *temp = minimum_node(node->right);
            if (temp->parent != node)
            {
                transplant(temp, temp->right);
                temp->right = node->right;
                temp->right->parent = temp;
            }
            transplant(node, temp);
            temp->left = node->left;
            temp->left->parent = temp;
        }

        node = nullptr;
    }
}

template <typename DataType>
Node<DataType>* Tree<DataType>::search_first(const DataType& data) throw( TException )
{
    Node<DataType>* node = root;

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
std::forward_list< Node<DataType>* > Tree<DataType>::search_all(const DataType& data) throw( TException )
{
    std::forward_list< Node<DataType>* > nodes;

    auto _search_all = [&nodes, data] (Node<DataType>* node)
    {
        if(node->data == data) nodes.push_front(node);
    };

    _clr_tree_traversial(root, _search_all);

    if(nodes.empty())
        throw TException("Узла с таким значением в данном дереве нет!");
    else
        return nodes;
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
    Node<DataType>* temp = find_place_to_add(subtree.root->data);

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
    Node<DataType>* node = search_first(data);

    return Tree<DataType>(node);
}

template <typename DataType>
void Tree<DataType>::clr_tree_traversial(std::function<void(Node<DataType>*)> function) noexcept
{
    _clr_tree_traversial(root, function);
}

template <typename DataType>
void Tree<DataType>::lcr_tree_traversial(std::function<void(Node<DataType>*)> function) noexcept
{
    _lcr_tree_traversial(root, function);
}

template <typename DataType>
void Tree<DataType>::lrc_tree_traversial(std::function<void(Node<DataType> *)> function) noexcept
{
    _lrc_tree_traversial(root, function);
}

template <typename DataType>
void Tree<DataType>::breadth_first_traversal(std::function<void(Node<DataType> *)> function) noexcept
{
    std::queue<Node<DataType>*> queue;
    queue.push(root);

    while(!queue.empty())
    {
        Node<DataType>* temp = queue.front();
        queue.pop();

        if(temp->left != nullptr)
            queue.push(temp->left);
        if(temp->right != nullptr)
            queue.push(temp->right);

        function(temp);

        temp = nullptr;
    }
}

template <typename DataType>
int Tree<DataType>::depth() noexcept
{
    return _depth(root);
}

template <typename DataType>
const Tree<DataType>& Tree<DataType>::operator=(const Tree<DataType>& other) noexcept
{
    if(root != nullptr)
    {
        free_resources(root);
    }

    auto func = [this](Node<DataType>* node)
    {
        push(node->data);
    };

    _clr_tree_traversial(other.root, func);

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
void Tree<DataType>::free_resources(Node<DataType>* node) noexcept
{
    auto func = [] (Node<DataType>* node)
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

    _lrc_tree_traversial(node,func);
}

template<typename DataType>
void Tree<DataType>::transplant(Node<DataType>* u,Node<DataType>* v) noexcept
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
Node<DataType>* Tree<DataType>::minimum_node(Node<DataType>* node) noexcept
{
    Node<DataType>* temp = node;

    while(temp->left != nullptr)
    {
        temp = temp->left;
    }

    return temp;
}

template <typename DataType>
Node<DataType>* Tree<DataType>::maximum_node(Node<DataType>* node) noexcept
{
    Node<DataType>* temp = node;

    while(temp->right != nullptr)
    {
        temp = temp->right;
    }

    return temp;
}

template <typename DataType>
Node<DataType>* Tree<DataType>::find_place_to_add(const DataType& data) noexcept
{
    Node<DataType>* node = root;

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
void Tree<DataType>::_clr_tree_traversial(Node<DataType>* node, std::function<void (Node<DataType>*)> function) noexcept
{
    function(node);

    if(node->left != nullptr)
        _clr_tree_traversial(node->left, function);

    if(node->right != nullptr)
        _clr_tree_traversial(node->right, function);
}

template <typename DataType>
void Tree<DataType>::_lcr_tree_traversial(Node<DataType>* node, std::function<void (Node<DataType>*)> function) noexcept
{
    if(node->left != nullptr)
        _lcr_tree_traversial(node->left, function);

    function(node);

    if(node->right != nullptr)
        _lcr_tree_traversial(node->right, function);
}

template <typename DataType>
void Tree<DataType>::_lrc_tree_traversial(Node<DataType>* node, std::function<void (Node<DataType>*)> function) noexcept
{
    if(node->left != nullptr)
        _lrc_tree_traversial(node->left, function);

    if(node->right != nullptr)
        _lrc_tree_traversial(node->right, function);

    function(node);
}

template <typename DataType>
int Tree<DataType>::_depth(Node<DataType>* node) noexcept
{
    if(node == nullptr) return 0;

    int left = 0;
    int right = 0;

    if(node->left != nullptr)
        left = _depth(node->left);
    else left = -1;

    if(node->right != nullptr)
        right = _depth(node->right);
    else right = -1;

    int max = left > right ? left : right;

    return max+1;
}

template <typename DataType>
void Tree<DataType>::_print_tree(Node<DataType>* node, int level) noexcept
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
