#include <iostream>
#include <stdexcept>
#include <cmath>
#include <string>
#include <vector>
#include <stack>
#include <memory>

#include "Calculator.h"
#include "Nodetypes.h"


/* Класс "Калькулятор"
 * 
 *
 * */
void Calculator::ShowHelp()
{
    cout << "\nConsole *** Kiss the Pig *** Calculator\n"
        << "----------------------------------------\n"
        << "Supports arithmetic, trigonometric, logarithmic, some statistic functions.\n";

    for (auto& i : nodetypes)
        i->ShowHelp();

    cout << "Operands inside function calls must be delimited by ;\n"
        << "You can build complex expressions like: \n\n"
        << "   ((2.2212+3*10) * max(10; 33+2^arithmean(12; ln(11); 5*6.23; 3^7); 11)) / (13 * 12)\n\n";
}

NodeType* Calculator::SearchNodeType(const string& str, const NodeType& prevnodetype, 
                                     int& pos, NodeRecognized& res) const
{
    res = NodeRecognized::No;
    int hypolen = -1;  //"стартовая длина" найденного Node вначале пооиска
    int maxpos = -1;  //перемення куда будет записываться макс знач длины строки
    int i = 0;
    while (i < nodetypes.size()) {
        maxpos = nodetypes[i]->CanIStartWith(str);
        //maxpos содержит индекс последнего имвола строки str удовлетвор-го NodeType
        if ((maxpos != -1) && (hypolen <= maxpos))
        {   //node type is recognized
            hypolen = maxpos;
            res = NodeRecognized::OK;
            //если опознанному объекту разрешено следовать за предыдущим то ОК
            if (&prevnodetype) {
                if (prevnodetype.CanBeFollowedBy(nodetypes[i]->name())) {
                    res = NodeRecognized::OKCanFollow;
                    break;
                }
            }
            else {
                res = NodeRecognized::OKCanFollow;
                break;
            }
        }

        ++i;
    }
    if (res == NodeRecognized::No) {
        pos = -1;
        return nodetypes[0];
    }
    else {
        pos = hypolen + 1;
        if (i < nodetypes.size())
            return nodetypes[i];
        else
            return nodetypes[0];
    }
}

void Calculator::AddNodeType(NodeType& nodetype)
{
    nodetypes.push_back(&nodetype);
}


Calculator::~Calculator() {

    int i = vec.size() - 1;
    while (i >= 0) { delete vec[i]; i--; }

    i = nodetypes.size() - 1;
    while (i >= 0) { delete nodetypes[i]; i--; }
  
}


//в обратную польскую нотацию (ОПЗ)
void Calculator::ToReversePolishNotation(string& str) {

    stack<Node*> stack;
    //параметры текущего Node
    NodeType* nodetype{ nullptr };
    Node* node{ nullptr };
    //параметры предыдущего Node
    NodeType* prevnodetype{ nullptr };
    Node* prevnode{ nullptr };
    NodeRecognized sntr;
    int nextpos{ 0 };
    string foundstr{ "" };

    //очистить вектор перед новым расчетом
    int i = vec.size() - 1;
    while (i >= 0) 
        delete vec[i], vec.pop_back(), i--;
    

    while (str.length()) {

        //опознать следующий кусок строки
        nodetype = SearchNodeType(str, (*prevnodetype), nextpos, sntr);
        //если не опознано то ошибка

        switch (sntr) {
        case NodeRecognized::No:
            throw string("Parsing error: unknown symbols starting with " +
                str.substr(0, 3) + "...'");
            break;
        case NodeRecognized::OK:
            foundstr = str.substr(0, nextpos);
            throw string("Parsing error: " + prevnode->getStrvalue() +
                " can't be followed by " + foundstr + ".");  //node->getStrvalue()
            break;
        case NodeRecognized::OKCanFollow:
            foundstr = str.substr(0, nextpos);
            node = new Node(*nodetype, foundstr);
            break;
        }

        //В зависимости от ТИПА КИДА разные сценарии
        switch (nodetype->name()) {

        case ParserName::Number:
            vec.push_back(node);
            break;
        case ParserName::UnaryOperation:
            stack.push(node);
            break;
        case ParserName::BinaryOperation:
            while (!stack.empty() && (stack.top()->getNodetypename() == ParserName::BinaryOperation ||
                stack.top()->getNodetypename() == ParserName::UnaryOperation)) {
                if (stack.top()->getPriority() >= (nodetype)->getPriority()) {
                    vec.push_back(stack.top());
                    stack.pop();
                }
                else
                    break;
            }
            stack.push(node);
            break;

        case ParserName::CompoundStart:
            stack.push(node);
            break;
        case ParserName::CompoundEnd: {
            int counter = 1;
            while ((!stack.empty() && (stack.top()->getNodetypename() != ParserName::CompoundStart)))
            {
                if (stack.top()->getNodetypename() == ParserName::OperandDelimiter) {
                    counter++;
                }
                else vec.push_back(stack.top());

                stack.pop();
            };
            //должны уже добраться до начало сcompounda (в этой точке)
            if (stack.empty())
                throw string("Parsing error: missed open bracket or function name.");
            else
            {// если (stack.top()->getNodetypename() == anCompoundStart ) {
                if (counter > MAXOPERANDCOUNT) throw string("Too many operands.");
                stack.top()->args = counter;
                vec.push_back(stack.top());
                stack.pop();
            }
            break;
        }


        case ParserName::OperandDelimiter:
            //если предыдущий аргумент Compounda сложный то перед тем как положить делимитер в стек нужно
            //вытащить в vector все операции относящиеся к предыд аргументу
            while (!stack.empty() && (stack.top()->getNodetypename() == ParserName::BinaryOperation ||
                (stack.top()->getNodetypename() == ParserName::UnaryOperation)))
            {  
                vec.push_back(stack.top());
                stack.pop();
            }
            //Если стек пуст значит встретился делимитер без compounda
            if (stack.empty())
                throw string("Parsing error: " + nodetype->getSymbolset() + " w/o function name.");
            stack.push(node);
            break;

        default:
            throw string("Unknown parsing error.");
            break;
        }

        str = str.substr(nextpos, str.length() - nextpos);
        prevnodetype = nodetype;
        prevnode = node;
    }

    //записать оставшееся в стеке в вектор
    while (!stack.empty()) {
        if (stack.top()->getNodetypename() == ParserName::CompoundStart) { // && !stack.top()->args) {
            throw string("Parsing error: missed close bracket.");
        }
        vec.push_back(stack.top());
        stack.pop();
    }

}


//Вычисление ОПЗ (Обратной польской нотации)
double Calculator::Execute()
{
    stack<double> stack;

    unsigned int v = 0;
    while (v < vec.size())
    {
        double operands[MAXOPERANDCOUNT];
        NodeType* nodetype = vec[v]->getNodetype();
        switch (vec[v]->getNodetypename()) {

        case ParserName::Number:
            //посчитать число (из строки в double) и положить его в стек
            stack.push(nodetype->Exec(nullptr, 1, vec[v]->getStrvalue()));
            break;

        case ParserName::UnaryOperation:
            if (stack.size() >= 1) {
                operands[0] = stack.top(); stack.pop();
                stack.push(nodetype->Exec(operands, 1, ""));
                break;
            }
            else
                throw string("Parsing error: must be 1 opnd for " +
                    dynamic_cast<UnaryOperation*>(nodetype)->getSymbolset() +
                    " operation.");

        case ParserName::BinaryOperation:
            if (stack.size() >= 2) {
                operands[1] = stack.top(); stack.pop();
                operands[0] = stack.top(); stack.pop();
                stack.push(nodetype->Exec(operands, 2, ""));
                break;
            }
            else
                throw string("Parsing error: must be 2 operands for " +
                    dynamic_cast<BinaryOperation*>(nodetype)->getSymbolset() +
                    " operation.");

        case ParserName::CompoundStart: {
            //проверить совпадает ли кол-во операндов необходимое Compound и тому что записано
            int i = dynamic_cast<CompoundStart*>(nodetype)->getoperandsneeded();
            if (i != 0 && vec[v]->args != i)
                throw string("Parsing error: opnd count mismatch.");
            //выбраь операнды из стека
            i = vec[v]->args - 1;
            while (i >= 0 && !stack.empty() && i < MAXOPERANDCOUNT) {
                operands[i] = stack.top();
                stack.pop();
                i--;
            }
            if (stack.empty() && i != -1)
                throw string("Parsing error: Not enough operands.");
            //посчитать Результат Compounda и положить его в стек
            stack.push(nodetype->Exec(operands, vec[v]->args, ""));
            break;
        }
        default:
            throw string("Error in parsing procedure.");
            break;

        }
        v++;
    }

    if (!stack.empty())
        return stack.top();
    else
        throw string("Unknown parsing error.");
};
   

