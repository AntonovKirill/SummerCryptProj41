#ifndef SEARCH_TREE_HPP
#define SEARCH_TREE_HPP

#include <iostream>
#include <vector>
#include <string>

class SearchNode
{
public:
    int num;
    int val;
    SearchNode* left;
    SearchNode* right;
    SearchNode* par;

    SearchNode(int n, int v, SearchNode* p = nullptr);
};


class SearchTree
{
public:
    SearchNode* root;
    int count;
    std::vector<int> assign;
    int steps;

    SearchTree(int vars);
    bool solve();
    bool backtrack(SearchNode* node);
    bool isComplete();
    bool checkSolution();
    int selectVariable();
    void printSolution();
};

#endif
