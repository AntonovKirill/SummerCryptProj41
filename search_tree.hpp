#ifndef SEARCH_TREE_HPP
#define SEARCH_TREE_HPP

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

   
    SearchNode(int n, int v, SearchNode* p = nullptr);
};


class SearchTree
{
private:
    SearchNode* root;         
    int count;                
    std::vector<int> assign;    
    int steps;                 
    MQS_system* system;         

   
    bool unitPropagation();
    bool checkEquation(const equation& eq);

public:
  
    SearchTree(MQS_system& sys);

  
    bool solve();
    bool backtrack(SearchNode* node);
    bool isComplete();
    bool checkSolution();
    int selectVariable();
    void printSolution();
};

#endif
