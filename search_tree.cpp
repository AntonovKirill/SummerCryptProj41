#include "search_tree.hpp"
#include <iostream>
#include <algorithm>

SearchNode::SearchNode(int n, int v, SearchNode* p)
{
    num = n;
    val = v;
    par = p;
    left = nullptr;
    right = nullptr;
}

SearchNode::~SearchNode()
{
    delete left;
    delete right;
}

SearchTree::SearchTree(MQS_system& sys)
{
    system = &sys;
    steps = 0;
    root = nullptr;

    int maxId = 0;
    for (const auto& lit : system->get_params())
    {
        if (lit.get_id() > maxId)
        {
            maxId = lit.get_id();
        }
    }
    varCount = (maxId + 1) / 2;

    saveState();
}

SearchTree::~SearchTree()
{
    delete root;
}

void SearchTree::saveState()
{
    stateStack.push({system->get_params(), system->get_equations()});
}

void SearchTree::restoreState()
{
    if (stateStack.size() < 2)
    {
        return;
    }
    
    stateStack.pop();
    
    if (!stateStack.empty())
    {
        applyState(stateStack.top().first, stateStack.top().second);
    }
}

void SearchTree::applyState(const std::vector<literal>& params,
                            const std::vector<equation>& equations)
{
    system->set_params(params);
    system->set_equations(equations);
}

bool SearchTree::solve()
{
    root = new SearchNode(-1, -1, nullptr);
    return backtrack(root);
}

bool SearchTree::backtrack(SearchNode* node)
{
    steps++;

    saveState();

    if (!unitPropagation())
    {
        restoreState();
        return false;
    }

    if (isComplete())
    {
        if (checkSolution())
        {
           
            return true;
        }
        restoreState();
        return false;
    }

    int v = selectVariable();
    if (v == -1)
    {
        restoreState();
        return false;
    }

    literal lit0(2 * v, 0);
    if (!system->add_literal_value(lit0))
    {
        SearchNode* child0 = new SearchNode(v, 0, node);
        node->left = child0;
        if (backtrack(child0))
        {
            return true;
        }
        delete child0;
        node->left = nullptr;
    }

    restoreState();
    saveState();

    literal lit1(2 * v, 1);
    if (!system->add_literal_value(lit1))
    {
        SearchNode* child1 = new SearchNode(v, 1, node);
        node->right = child1;
        if (backtrack(child1))
        {
            return true;
        }
        delete child1;
        node->right = nullptr;
    }

    restoreState();
    return false;
}

bool SearchTree::unitPropagation()
{
    bool conflict = system->unit_propagation();
    return !conflict;
}

bool SearchTree::isComplete()
{
    auto params = system->get_params();
    
    std::vector<bool> known(varCount, false);
    for (const auto& lit : params)
    {
        int id = lit.get_id();
        if (id % 2 == 0 && id / 2 < varCount && lit.is_known())
        {
            known[id / 2] = true;
        }
    }
    
    for (int i = 0; i < varCount; ++i)
    {
        if (!known[i])
        {
            return false;
        }
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
    auto params = system->get_params();
    int xorSum = 0;

    for (int rawId : eq.get_ids())
    {
        int var = rawId / 2;
        int litId = 2 * var;  // id для xi

        bool known = false;
        int val = 0;
        for (const auto& lit : params)
        {
            if (lit.get_id() == litId && lit.is_known())
            {
                known = true;
                val = lit.get_value();
                break;
            }
        }
        if (!known)
        {
            return false;
        }

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
    auto params = system->get_params();
  
    std::vector<bool> known(varCount, false);
    for (const auto& lit : params)
    {
        int id = lit.get_id();
        if (id % 2 == 0 && id / 2 < varCount && lit.is_known())
        {
            known[id / 2] = true;
        }
    }
    
    std::vector<int> freq(varCount, 0);
    for (const auto& eq : system->get_equations())
    {
        for (int rawId : eq.get_ids())
        {
            int v = rawId / 2;
            if (v < varCount && !known[v])
            {
                freq[v]++;
            }
        }
    }

    int best = -1;
    int bestFreq = -1;
    for (int i = 0; i < varCount; ++i)
    {
        if (!known[i] && freq[i] > bestFreq)
        {
            bestFreq = freq[i];
            best = i;
        }
    }

    if (best != -1)
    {
        return best;
    }

    for (int i = 0; i < varCount; ++i)
    {
        if (!known[i])
        {
            return i;
        }
    }

    return -1;
}


void SearchTree::printSolution()
{
    std::cout << "Решение найдено!" << std::endl;
    auto params = system->get_params();
    
    std::vector<int> solution(varCount, -1);
    for (const auto& lit : params)
    {
        int id = lit.get_id();
        if (id % 2 == 0 && id / 2 < varCount && lit.is_known())
        {
            solution[id / 2] = lit.get_value();
        }
    }
    
    for (int i = 0; i < varCount; ++i)
    {
        if (solution[i] != -1)
        {
            std::cout << "x" << i << " = " << solution[i] << std::endl;
        }
        else
        {
            std::cout << "x" << i << " = ? (не назначена)" << std::endl;
        }
    }
    std::cout << "Количество шагов: " << steps << std::endl;
}
