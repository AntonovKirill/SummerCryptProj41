#ifndef SEARCH_TREE_HPP
#define SEARCH_TREE_HPP

#include "equations_system.hpp"

class SearchNode
{
public:
    int num; // номер переменной 
    int value;            
    SearchNode* left;
    SearchNode* right;
    SearchNode* parent;
    
    // КОНСТРУКТОР (создает узел)
    SearchNode(int n, int val, SearchNode* p = nullptr)
    {
        num = n;
        value = val;
        parent = p;
        left = nullptr;
        right = nullptr;
    }
};

#endif
