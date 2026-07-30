#ifndef SEARCH_TREE_HPP
#define SEARCH_TREE_HPP

#include <vector>
#include <stack>
#include <utility>
#include "equations_system.hpp"

class SearchNode
{
public:
    int num;                     
    int val;                     
    SearchNode* left;
    SearchNode* right;
    SearchNode* par;

    SearchNode(int n, int v, SearchNode* p = nullptr);
    ~SearchNode();
};

class SearchTree
{
private:
    SearchNode* root;
    int varCount;
    int steps;
    MQS_system* system;

    std::stack< std::pair< std::vector<literal>, std::vector<equation> > > stateStack;

    bool unitPropagation();
    bool checkEquation(const equation& eq);
    void saveState();
    void restoreState();
    void applyState(const std::vector<literal>& params, const std::vector<equation>& equations);

public:
    SearchTree(MQS_system& sys);
    ~SearchTree();

    bool solve();
    bool backtrack(SearchNode* node);
    bool isComplete();
    bool checkSolution();
    int selectVariable();
    void printSolution();
};

#endif
