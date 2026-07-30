#include "search_tree.hpp"
#include <iostream>

SearchNode::SearchNode(int n, int v, SearchNode* p)
{
    num = n;
    val = v;
    par = p;
    left = nullptr;
    right = nullptr;
}

SearchTree::SearchTree(MQS_system& sys)
{
    system = &sys;
    count = system->get_params().size();
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

    if (!unitPropagation())
    {
        return false;  
    }

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

bool SearchTree::unitPropagation()
{
    bool changed = true;
    while (changed)
    {
        changed = false;

        for (const auto& eq : system->get_equations())
        {
            int unknownVar = -1;
            int unknownCount = 0;
            int xorSum = 0;

            for (int rawId : eq.get_ids())
            {
                int var = rawId / 2;
                int val = assign[var];

                if (val == -1)
                {
                    unknownVar = var;
                    unknownCount++;
                }
                else
                {
                    if (rawId % 2 == 1) 
                    {
                        val = 1 - val;
                    }
                    xorSum ^= val;
                }
            }

            if (unknownCount == 1)
            {
                assign[unknownVar] = xorSum;
                changed = true;
            }
            else if (unknownCount == 0 && xorSum != 0)
            {
                return false;  
            }
        }
    }
    return true;
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
    for (const auto& eq : system->get_equations())
    {
        if (!checkEquation(eq))
        {
            return false;
        }
    }
    return true;
}

bool SearchTree::checkEquation(const equation& eq)
{
    int xorSum = 0;

    for (int rawId : eq.get_ids())
    {
        int var = rawId / 2;
        int val = assign[var];

        if (val == -1) return false;  

        if (rawId % 2 == 1)
        {
            val = 1 - val;
        }

        xorSum ^= val;
    }

    return xorSum == 0;
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
