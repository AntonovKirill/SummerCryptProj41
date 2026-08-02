#ifndef SEARCH_TREE_HPP
#define SEARCH_TREE_HPP

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
public:
    SearchNode* root;
    int varCount;
    int steps;
    MQS_system* system;

    std::stack< MQS_system > stateStack;

    std::vector<int> inputVars;

    bool checkEquation(const equation& eq) const;
    void saveState();
    void restoreState();
    void applyState(const MQS_system& MQS_system1);

    SearchTree(MQS_system& sys);
    ~SearchTree();

    bool solve();
    bool backtrack(SearchNode* node);
    bool isComplete() const;
    bool checkSolution() const;
    int selectVariable() const;
    void printSolution() const;
};

#endif
