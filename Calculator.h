#ifndef _CALCULATOR_H_
#define _CALCULATOR_H_

#include <vector>
#include <string>
#include "Nodetypes.h"


class NodeType; 
class Node;


using namespace std;




class Calculator
{
    //массив типов узлов на основании которых Парсер калькулятора ведет разбор
    vector<NodeType*> nodetypes;
    //массив узлов, формируется методом ToReversePolishNotation (парсером) из введенноой строки
    vector<Node*> vec;
protected:
public:
    
    Calculator() { };
    virtual ~Calculator();
    NodeType* SearchNodeType(const string& str, const NodeType& prevnodetype,
        int& pos, NodeRecognized& res) const;
    void AddNodeType(NodeType& nodetype);
    void ShowHelp();
    //в обратную польскую нотацию (ОПЗ)
    void ToReversePolishNotation(string& str);
    //Вычисление ОПЗ (Обратной польской нотации)
    long double Execute();
};

#endif