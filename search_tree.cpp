#include "search_tree.hpp"

SearchNode::SearchNode(int n, int v, SearchNode* p)
{
    num = n;
    val = v;
    par = p;
    left = nullptr;
    right = nullptr;
}

SearchTree::SearchTree(int vars)
{
    count = vars;
    steps = 0;
    root = nullptr;
    assign.resize(count, -1);
}

bool SearchTree::solve()
{
    root = new SearchNode(-1, -1, nullptr);
    return backtrack(root);
}

bool SearchTree::backtrack(SearchNode* node)
{
    steps++;

    if (isComplete())
    {
        return checkSolution();
    }

    int v = selectVariable();
    if (v == -1) return false;

    assign[v] = 0;
    SearchNode* child0 = new SearchNode(v, 0, node);
    node->left = child0;
    if (backtrack(child0)) return true;

    assign[v] = -1;
    delete child0;
    node->left = nullptr;

    assign[v] = 1;
    SearchNode* child1 = new SearchNode(v, 1, node);
    node->right = child1;
    if (backtrack(child1)) return true;

    assign[v] = -1;
    delete child1;
    node->right = nullptr;

    return false;
}

bool SearchTree::isComplete()
{
    for (int i = 0; i < count; i++)
    {
        if (assign[i] == -1) return false;
    }
    return true;
}

bool SearchTree::checkSolution()
{
    // ПОКА ЗАГЛУШКА
    return true;
}

int SearchTree::selectVariable()
{
    for (int i = 0; i < count; i++)
    {
        if (assign[i] == -1) return i;
    }
    return -1;
}

void SearchTree::printSolution()
{
    std::cout << "Решение найдено!" << std::endl;
    for (int i = 0; i < count; i++)
    {
        std::cout << "x" << i << " = " << assign[i] << std::endl;
    }
    std::cout << "Шагов: " << steps << std::endl;
}
