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

class SearchTree
{
private:
    SearchNode* root;       
    MQS_system* system;     
    int varCount; //кол-во переменных
    int stepCount; // сколько шагов сделали
    
public:
    // КОНСТРУКТОР
    SearchTree(MQS_system& sys)
    {
        system = &sys;
        varCount = sys.getParamNum().size(); // получаем число переменных
        root = nullptr;
        stepCount = 0;
    }
    
    bool solve()
    {
        root = new SearchNode(-1, -1, nullptr);
        return backtrack(root);
    }
    
    // рекурсивный обход
    bool backtrack(SearchNode* node)
    {
        stepCount++;
        
        // Проверяем, все ли переменные назначены
        if (isComplete())
        {
            return checkSolution(); 
        }
        
        int nextVar = selectVariable();
        if (nextVar == -1)
        {
            return false;  
        }
        
        // x = 0
        SearchNode* child0 = new SearchNode(nextVar, 0, node);
        node->left = child0;
        if (backtrack(child0))
        {
            return true; 
        }
        delete child0; 
        // x = 1
        SearchNode* child1 = new SearchNode(nextVar, 1, node);
        node->right = child1;
        if (backtrack(child1))
        {
            return true;  
        }
        delete child1;
        
        return false;
    }
    
    bool isComplete()
    {
      // здесь будет проверка
        return true;
    }
    
    bool checkSolution()
    {
        // здесь будет проверка уравнений
        return true;
    }
    

};

#endif
