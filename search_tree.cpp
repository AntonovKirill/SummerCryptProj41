#include "search_tree.hpp"
#include <iostream>

SearchNode::SearchNode(int n, int v, SearchNode *p)
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

SearchTree::SearchTree(MQS_system &sys)
{
    system = &sys;
    steps = 0;
    root = nullptr;
    int maxId = 0;
    for (const auto &lit : sys.params)
    {
        if (lit.get_id() > maxId)
        {
            maxId = lit.get_id();
        }
    }
    varCount = (maxId + 1) / 2;
    inputVars = sys.input_vars;
    saveState();
}

SearchTree::~SearchTree()
{
    delete root;
}

void SearchTree::saveState()
{
    stateStack.push(*system);
}

void SearchTree::restoreState()
{
    if (stateStack.empty())
    {
        return;
    }
    stateStack.pop();
    if (!stateStack.empty())
    {
        applyState(stateStack.top());
    }
}

void SearchTree::applyState(const MQS_system &MQS_system1)
{
    system->params = MQS_system1.get_params();
    system->equations = MQS_system1.get_equations();
}

bool SearchTree::backtrack(SearchNode *node)
{
    steps++;
    // std::cout << "1 " << steps << " " << stateStack.size() << " " << std::endl; // выводит номер текущего шага и размер стека состояний
    if (checkSolution()) // сначала проверяем, выполнено ли текущее состояние (все уравнения исчезли)
    {
        // std::cout << "2 solution " << steps << " " << stateStack.size() << std::endl; // нашла решение вверху функции
        return true;
    }
    // выбираем следующую еще не назначенную переменную
    int v = selectVariable();
    if (v == -1) // свободных переменных нет
    {
        return false; // неназначенных переменных не осталось (или осталась невязка в уравнениях)
    }
    // ВЕТВЬ 0 (x_i = 0)
    saveState(); // сохраняме текущее состояние системы
    // std::cout << "4 branching " << v << " = 0" << std::endl; // решение о ветвлении 0
    literal lit0(2 * v, 0);
    if (!system->unit_propagation(lit0)) // если unit_prop не вернул ошибку
    {
        SearchNode *child0 = new SearchNode(v, 0, node);
        node->left = child0;
        if (backtrack(child0))
        {
            return true; // тут еще был маркер успешного возврата (номер 5), я его убрала
        }
        delete child0;
        node->left = nullptr;
    }
    restoreState();
    // ВЕТВЬ 1 (x_i = 1)
    saveState();
    // std::cout << "6 branching " << v << " = 1" << std::endl; // выбрали ветку 1
    literal lit1(2 * v, 1);
    if (!system->unit_propagation(lit1))
    {
        SearchNode *child1 = new SearchNode(v, 1, node);
        node->right = child1;
        if (backtrack(child1))
        {
            return true; // тут еще был маркер успешного возврата (номер 7), я его убрала
        }
        delete child1;
        node->right = nullptr;
    }
    restoreState();
    // std::cout << "8 " << steps << " contradiction" << std::endl; // маркер противоречия/тупика
    return false;
}

bool SearchTree::isComplete() const
{
    for (int i = 1; i < varCount; ++i)
    {
        int raw_id = 2 * i;
        if (raw_id < (int)system->params.size())
        {
            if (!system->params[raw_id].is_known())
            {
                return false;
            }
        }
    }
    return true;
}

bool SearchTree::checkSolution() const
{
    if (system->equations.empty()) // Если уравнений не осталось вообще — решение точно найдено!
    {
        return true;
    }
    // Проверяем все оставшиеся уравнения
    for (const auto &eq : system->equations)
    {
        if (!checkEquation(eq))
        {
            return false;
        }
    }
    return true;
}

bool SearchTree::solve()
{
    // проверяем, решена ли система прямо на старте (включая unit_propagation)
    if (checkSolution())
    {
        return true;
    }
    // запускаем поиск
    SearchNode *rootNode = new SearchNode(0, 0, nullptr);
    root = rootNode;
    return backtrack(rootNode);
}

bool SearchTree::checkEquation(const equation &eq) const
{
    // КВАДРАТИЧНОЕ УРАВНЕНИЕ (x = y AND z)
    if (!eq.get_Is_line() && eq.ids.size() == 3)
    {
        int y_raw = eq.ids[0]; // 2 (x1)
        int z_raw = eq.ids[1]; // 4 (x2)
        int x_raw = eq.ids[2]; // 6 (x3)
        int y_val = system->params[y_raw].get_value() ^ (y_raw % 2);
        int z_val = system->params[z_raw].get_value() ^ (z_raw % 2);
        int x_val = system->params[x_raw].get_value() ^ (x_raw % 2);
        return x_val == (y_val & z_val);
    }
    // ЛИНЕЙНОЕ УРАВНЕНИЕ (XOR)
    int xorSum = 0;
    for (int rawId : eq.ids)
    {
        if (!system->params[rawId].is_known())
        {
            return false;
        }
        int val = system->params[rawId].get_value();
        if (rawId % 2 == 1)
        {
            val = 1 - val;
        }
        xorSum ^= val;
    }
    // Линейные уравнения из 1 элемента (секция "lin 1") равны 1.
    // Если в уравнении больше элементов (XOR = 0), проверяем на 0.
    if (eq.ids.size() == 1)
    {
        return xorSum == 1;
    }
    return xorSum == 0;
}

int SearchTree::selectVariable() const
{
    const auto &params = system->params;
    std::vector<bool> known(varCount, false);
    for (const auto &lit : params)
    {
        int id = lit.get_id();
        if (id % 2 == 0 && id / 2 < varCount && lit.is_known())
        {
            known[id / 2] = true;
        }
    }
    std::vector<bool> isInput(varCount, false);
    for (int inputVarId : inputVars)
    {
        int varIndex = inputVarId / 2;
        if (varIndex < varCount)
        {
            isInput[varIndex] = true;
        }
    }
    std::vector<int> freq(varCount, 0);
    for (const auto &eq : system->equations)
    {
        for (int rawId : eq.ids)
        {
            int v = rawId / 2;

            if (v < varCount && !known[v] && isInput[v])
            {
                freq[v]++;
            }
        }
    }
    int best = -1;
    int bestFreq = -1;
    for (int i = 0; i < varCount; ++i)
    {
        if (isInput[i] && !known[i] && freq[i] > bestFreq)
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
        if (isInput[i] && !known[i])
        {
            return i;
        }
    }
    return -1;
}

void SearchTree::printSolution() const
{
    std::cout << "Решение найдено!" << std::endl;
    const auto &params = system->params;
    std::vector<int> solution(varCount, -1);
    for (const auto &lit : params)
    {
        int id = lit.get_id();
        if (id % 2 == 0 && id / 2 < varCount && lit.is_known())
        {
            solution[id / 2] = lit.get_value();
        }
    }
    for (int i = 1; i < varCount; ++i)
    {
        if (solution[i] != -1)
        {
            std::cout << "x" << i << " = " << solution[i] << std::endl;
        }
        else
        {
            std::cout << "x" << i << " = 0 (свободная)" << std::endl; // Свободная переменная: выводим по умолчанию 0 (с пометкой свободного выбора)
        }
    }
    std::cout << "Количество шагов: " << steps << std::endl;
}
