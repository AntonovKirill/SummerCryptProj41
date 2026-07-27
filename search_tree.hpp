#ifndef SEARCH_TREE_HPP
#define SEARCH_TREE_HPP

#include <iostream>
#include <vector>
#include "equations_system.hpp"

class SearchNode
{
public:
    int num;
    int val;
    SearchNode* left;
    SearchNode* right;
    SearchNode* par;

    SearchNode(int n, int v, SearchNode* p = nullptr)
    {
        num = n;
        val = v;
        par = p;
        left = nullptr;
        right = nullptr;
    }
};

class SearchTree
{
private:
    SearchNode* root;
    int count;
    std::vector<int> assign;
    int steps;
    MQS_system* system; 

public:
    SearchTree(MQS_system& sys)
    {
        system = &sys;
        count = system->get_params().size();
        steps = 0;
        root = nullptr;
        assign.resize(count, -1);
    }

    bool solve()
    {
        root = new SearchNode(-1, -1, nullptr);
        return backtrack(root);
    }

    bool backtrack(SearchNode* node)
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

    bool isComplete()
    {
        for (int i = 0; i < count; i++)
        {
            if (assign[i] == -1) return false;
        }
        return true;
    }

    bool checkSolution()
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

    bool checkEquation(const equation& eq)
    {
        bool result = 0;
        int xorSum = 0;

        for (const auto& lit : eq.get_param_form())
        {
            int var = lit.get_id() / 2;
            int val = assign[var];

            if (val == -1) return false; 

            if (lit.get_id() % 2 == 1)
            {
                val = 1 - val;
            }

            xorSum ^= val;
        }

        return xorSum == 0;
    }

    int selectVariable()
    {
        for (int i = 0; i < count; i++)
        {
            if (assign[i] == -1) return i;
        }
        return -1;
    }

    void printSolution()
    {
        std::cout << "Решение найдено!" << std::endl;
        for (int i = 0; i < count; i++)
        {
            std::cout << "x" << i << " = " << assign[i] << std::endl;
        }
        std::cout << "Шагов: " << steps << std::endl;
    }
};

#endif
