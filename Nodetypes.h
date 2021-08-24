#ifndef _NODETYPES_H_
#define _NODETYPES_H_

#include <algorithm>

constexpr int MAXFOLLOWEDBY = 10;
constexpr int MAXOPERANDCOUNT = 50;
constexpr long double TooSmallNumber = 10E-20;


using namespace std;


enum class ParserName {
    Unknown,
    NodeType,
    OperandDelimiter,
    Number,
    UnaryOperation,
    BinaryOperation,
    CompoundStart,
    CompoundEnd
};

//приоритет опеераций
enum class Priority {
    Undefined,
    Low,
    Middle,
    High
};


enum class NodeRecognized { No, OK, OKCanFollow };


class NodeType
{
protected:
    Priority _priority;
    unsigned int _followedbycount;
    ParserName _followedby[MAXFOLLOWEDBY]{};
    string _symbolset;
    int _len;
    string _about{};
    string _usecase{};
public:
    NodeType(string symbolset, int len ) :
        _priority(Priority::Undefined), 
        _symbolset(symbolset), 
        _len(len), _followedbycount(0)
    { };
    //конструктор копии
    //NodeType(const NodeType& original) = delete;
    virtual ~NodeType() { };
    void ShowHelp() const
    {
        if (!_about.empty()) 
        {
            string s = "  " + _about + "                                     ";
            s.resize(40); s += _usecase;
            cout << s << endl;
        }
    }
    const Priority getPriority() const { return _priority; };
    virtual ParserName name() const { return ParserName::NodeType; };
    virtual bool CanBeFollowedBy(ParserName name) const {
        for (unsigned int i = 0; i < _followedbycount; i++) {
            //.find и другие методы возвращаюют тип string::size_type
            if (_followedby[i] == name) return true;
        }
        return false;
    }
    string getSymbolset() const { return _symbolset; };
    //функция "CanIStartWith" возвращает
    // -1 если строка-параметр не может быть объектом этого потомка Nodetype
    //или позицию в строке которвая явл-ся следующей за последней позицией подстроки которая явл-ся 
    //данным класом: для класса Number CanIStartWith("12+23") вернет 3
    virtual int CanIStartWith(string str) const
    {
        if (_len == 0) {
            string::size_type i = 0;
            while (i <= str.size() - 1) {
                if (_symbolset.find(str.at(i)) == string::npos)  break;
                i++;
            }
            return i - 1;
        }
        else if (_len == 1)
        {
            if (_symbolset.find(str.at(0)) == string::npos)
                return -1;
            else
                return 0;
        }

        return -1;  //error
    }
    /*  opnd - сслыка на массив аргументов, 
        opndcount - их кол-во
        str - строка (только для простых типов (Number) непосредственно преобразуемых в число)
    */
    virtual long double Exec (const long double opnd[], int opndcount, string str) const { return 0.0; };
};



class Number : public NodeType
{
public:
    Number() : NodeType(".0123456789", 0)
    {
        _priority = Priority::Undefined;
        _followedby[0] = ParserName::BinaryOperation;
        _followedby[1] = ParserName::OperandDelimiter;
        _followedby[2] = ParserName::CompoundEnd;
        _followedbycount = 3;
    };
    ParserName name() const override { return ParserName::Number; };
    long double Exec(const long double opnd[], int opndcount, string str) const override
    {
        return stof(str);   //из строки в long double
    }
};

class BinaryNumber : public NodeType
{
public:
    BinaryNumber() : NodeType("01", 0)
    {
        _priority = Priority::Undefined;
        _followedby[0] = ParserName::BinaryOperation;
        _followedby[1] = ParserName::OperandDelimiter;
        _followedby[2] = ParserName::CompoundEnd;
        _followedbycount = 3;

        _about = "0b101110";
        _usecase = "Binary number, without fraction part";
    };
    ParserName name() const override { return ParserName::Number; };
    int CanIStartWith(string str) const override
    {
        if (str.substr(0, 2) == "0b")
        {
            int i = 2;
            while (i < str.length() && (str.at(i) == '1' | str.at(i) == '0'))
                ++i;
            if (i >= 3) return i - 1;
            else return -1;
        }
        else return -1;
    }

    long double Exec(const long double opnd[], int opndcount, string str) const override
    {
        //из строки 0b0111011101 в long double
        int i = str.length() - 1;
        long double sum{ 0.0 };
        while (i >= 2)
            sum += str.at(i) == '0' ? 0 : pow(2, str.length() - 1 - i), --i;
        return sum;
    };
};


class HexNumber : public NodeType
{
public:
    HexNumber() : NodeType("0123456789ABCDEF", 0)
    {
        _priority = Priority::Undefined;
        _followedby[0] = ParserName::BinaryOperation;
        _followedby[1] = ParserName::OperandDelimiter;
        _followedby[2] = ParserName::CompoundEnd;
        _followedbycount = 3;

        _about = "0x1AF233C";
        _usecase = "Hex number, without fraction part";
    };
    ParserName name() const override { return ParserName::Number; };
    int CanIStartWith(string str) const override
    {
        if (str.substr(0, 2) == "0x")
        {
            int i = 2;
            while (i < str.length() && (_symbolset.find(str.at(i)) != string::npos))
                ++i;
            if (i >= 3) return i - 1;
            else return -1;
        }
        else return -1;
    }

    long double Exec(const long double opnd[], int opndcount, string str) const override
    {
        //из строки 0x в long double
        int i = str.length() - 1;
        long double sum{ 0.0 };
        while (i >= 2) {
            if (str.at(i) >= 65)
                sum += (str.at(i) - 55) * pow(16, str.length() - 1 - i);
            else
                sum += (str.at(i) - 48) * pow(16, str.length() - 1 - i);
            --i;
        }
        return sum;
    };
};


class Constant: public NodeType
{
public:
    Constant(string symbols) : NodeType(symbols, 0)
    {   _priority=Priority::Undefined;
        _followedby[0] = ParserName::BinaryOperation;
        _followedby[1] = ParserName::OperandDelimiter;
        _followedby[2] = ParserName::CompoundEnd;
        _followedbycount =3;
    };
    ParserName name()  const override { return ParserName::Number; };
    int CanIStartWith(string str)  const override
    {
        if (str.find(_symbolset) == 0)
            return _symbolset.length()-1;
        else
            return -1;
    }
    long double Exec(const long double opnd[], int opndcount, string str) const override { return 0.0;}
};

class BinaryOperation : public NodeType {
public:
    BinaryOperation(string sign, Priority priority ): NodeType (sign, 1 ) {
        _priority=priority;
        _symbolset=sign;
        _followedby[0] = ParserName::Number;
        _followedby[1] = ParserName::CompoundStart;
        //_followedby[2] = ParserName::UnaryOperation;
        _followedbycount =2;
    }
    ParserName name() const override { return ParserName::BinaryOperation; } ;
};

class UnaryOperation : public NodeType {
public:
    UnaryOperation (string symbolset, Priority priority ): NodeType(symbolset, 1) {
        this->_priority=priority;
        _followedby[0] = ParserName::Number;
        _followedby[1] = ParserName::CompoundStart;
        _followedby[2] = ParserName::CompoundEnd;
        _followedby[3] = ParserName::BinaryOperation;
        _followedby[4] = ParserName::UnaryOperation;
        _followedbycount =5;
     };
    ParserName name()  const override { return ParserName::UnaryOperation; } ;
};



//COmpound - абстрактный класс на основе котрого строятся все "сложные" , составные классы
//строки которых заключены между скобками или чемто похожим , (и функции)
class CompoundStart : public NodeType
{
protected :
    //колво операндов которое должно быть у compounda (если равно нулю - то сколькко угодно)
    int _operandsneeded= 0;
public:
    CompoundStart(string symbolset, int operandsneeded) : NodeType(symbolset, 0){
        _operandsneeded=operandsneeded;
        _priority=Priority::High;
        _symbolset = symbolset;
        _followedby[0] = ParserName::UnaryOperation;
        _followedby[1] = ParserName::Number;
        _followedby[2] = ParserName::CompoundStart;
        _followedbycount = 3;
       }
    int getoperandsneeded() const { return _operandsneeded; };
    ParserName name()  const override { return ParserName::CompoundStart; } ;
    long double Exec(const long double opnd[], int opndcount, string str) const override {
        return opnd[0];
    };

    int CanIStartWith(string str) const override
    {
        //если признак начала Compoundа стоит вначале
        if (str.find(_symbolset) == 0)
            return _symbolset.length()-1;
        else
            return -1;
    }

};

class CompoundEnd : public NodeType
{
public:
    CompoundEnd(string symbolset) : NodeType(symbolset, 0){
        _priority=Priority::High;
        _followedby[0] = ParserName::UnaryOperation;
        _followedby[1] = ParserName::BinaryOperation;
        _followedby[2] = ParserName::CompoundEnd;
        _followedby[3] = ParserName::OperandDelimiter;
        _followedbycount =4;
       }
    ParserName name()  const override { return ParserName::CompoundEnd; } ;
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        return opnd[0];
    };
    int CanIStartWith(string str) const override
    {
        //если признак начала Compoundа стоит вначале
        if (str.find(_symbolset) == 0)
            return _symbolset.length()-1;
        else
            return -1;
    }


};

//разделитель аргументов в функциях
class OperandDelimiter : public NodeType
{
public:
    OperandDelimiter(string symbolset): NodeType(symbolset, 1)  {
        _followedby[0] = ParserName::Number;
        _followedby[1] = ParserName::CompoundStart;
        _followedby[2] = ParserName::UnaryOperation;
        _followedbycount =3;
    };
    ParserName name()  const override { return ParserName::OperandDelimiter;}
};



// узел содержащий конкретную инфиормацию для вычисления
//операцию либо число
class Node
{
    string _strvalue;
    NodeType* _nodetype;
protected:
public:
    int args = 0; // кол-во аргументов (заполняется при вычислении ОПН)
    Node(NodeType& nodetype, string strvalue) : 
        _strvalue(strvalue), _nodetype(&nodetype)
    {}
    string getStrvalue() const { return _strvalue; };
    NodeType* getNodetype() const { return _nodetype; };
    ParserName getNodetypename() const { return _nodetype->name(); };
    Priority getPriority() const { return _nodetype->getPriority(); };
};

    /*
     * Классы операций (унарных и бинарных и функции)
     * **********************************************************
     * которые строятся на базе абстрактных классов
     * Парсер калькулятора () работает
     * на уровне абстрактных классов, т.к. информации которые
     * они предоставляют достаточно для разбора.
     * пользователь (программер) может определять свои
     * =классы-операции (наследуя от BinaryOperation, UnaryOperation)
     * =классы-числа (наследуя от Number или Constant или напрямую от NodeType)
     * =классы-функции (наследуя от СompoundStart)
     *
     */

//унарные и бинарные
//*******************************************

class UnaryMinus: public UnaryOperation{
public:
    UnaryMinus() : UnaryOperation ("-",Priority::Low) {
        _followedby[0] = ParserName::Number;
        _followedby[1] = ParserName::CompoundStart;
        _followedby[2] = ParserName::UnaryOperation;
        _followedbycount =3;
    };
    long double Exec(const long double opnd[], int opndcount, string str) const override {
        return ( - opnd[0]);
    };
};

class Addition: public BinaryOperation {
public:
    Addition() : BinaryOperation ("+",Priority::Low) {};
    long double Exec(const long double opnd[], int opndcount, string str)  const override {
        long double op2 = opnd[0] + *(opnd + 1);
        return ( op2 );
    };
};

class Substraction: public BinaryOperation {
public:
    Substraction() : BinaryOperation("-", Priority::Low) {};
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        long double op2= opnd[0] - (*(opnd+1));
        return ( op2 );
    };
};


class Multiplication: public BinaryOperation {
public:
    Multiplication() : BinaryOperation ("*",Priority::Middle) {};
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        long double op2 = opnd[0] * (*(opnd + 1));
        return ( op2 );
    };
};

class Division: public BinaryOperation {
public:
    Division() : BinaryOperation ("/",Priority::Middle) {};
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        if (abs(*(opnd+1)) < TooSmallNumber)
            throw string ("Division: divider is zero or too small.");
        return  opnd[0] / (*(opnd+1));
    };
};


class BitAND: public BinaryOperation {
public:
    BitAND() : BinaryOperation("&", Priority::High) 
    {
        _about = "X & Y";
        _usecase = "Bit AND";
    };
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        long long arg1 = round(opnd[0]);
        long long arg2 = round(*(opnd + 1));
        return  arg1 & arg2;
    };
};

class BitOR : public BinaryOperation {
public:
    BitOR() : BinaryOperation("|", Priority::Middle) 
    {
        _about = "X | Y";
        _usecase = "Bit OR";
    };
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        long long arg1 = round(opnd[0]);
        long long arg2 = round(*(opnd + 1));
        return  arg1 | arg2;
    };
};

class BitXOR : public BinaryOperation {
public:
    BitXOR() : BinaryOperation("^", Priority::Middle) 
    {
        _about = "X ^ Y";
        _usecase = "Bit XOR";
    };
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        unsigned long long arg1 = round(opnd[0]);
        unsigned long long arg2 = round(*(opnd + 1));
        return  arg1 ^ arg2;
    };
};

class BitNOT : public UnaryOperation {
public:
    BitNOT() : UnaryOperation("~", Priority::High)
    {
        _about = "~ X";
        _usecase = "Bit NOT";
    };
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        long long arg = round(opnd[0]);
        return  ~arg;
    };
};

class Factorial: public UnaryOperation {
public:
    Factorial() : UnaryOperation("!",Priority::High) 
    {
        _about = "!X"; 
        _usecase = "Factorial of X";
    };
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        long double result = 1;
        for (int i = 1; i <= round(opnd[0]); i++) {
            result *= i;
        }
        return result;
    };
};

/* Классы функций
 * Функция начинается с имени с прилегающей скобкой и заканчиваться закрывающей скобкой.
 * В конструктор передается "имя(" и кол-во аргументов которые принимает функция (если =0 то
 * любое кол-во арументов
 * Функция имеет вид name(p1;p2;...pn)
 * где р[n] - аргумент(ы). Колво аргументов >=1 и
 * ";" - это разделитель аргументов.
 * */

class Abs : public CompoundStart {
public:
    Abs() : CompoundStart("abs(",1) 
    {
        _about = "abs(X)"; 
        _usecase = "Absolule value of X";
    };
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        return abs(*(opnd));
    };
};

class Trunc : public CompoundStart {
public:
    Trunc() : CompoundStart("trunc(", 1)
    {
        _about = "trunc(X)"; 
        _usecase = "Eliminates fractional part of X";
    };
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        return trunc(*(opnd));
    };
};

class Round : public CompoundStart {
public:
    Round() : CompoundStart("round(", 1)
    {
        _about = "round(X)"; 
        _usecase = "Rounds X to nearest integer number";
    };
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        return trunc(*(opnd));
    };
};

class Pow : public CompoundStart {
public:
    Pow() : CompoundStart("pow(", 2)
    {
        _about = "pow(X, Y)";
        _usecase = "Pow(X, Y). Ex: pow(2, 3) = 8";
    };
    long double Exec(const long double opnd[], int opndcount, string str)  const override
    {
        long double op2 = pow(opnd[0], *(opnd + 1));
        return (op2);
    };
};

class Pi : public Constant {
public:
    Pi() : Constant("pi")
    {
        _about = "pi";
        _usecase = "Pi = 3.14159265358979...";
    }
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        return 3.14159265358979323846264338327;
    }
};


class Sines: public CompoundStart {
public:
    Sines() : CompoundStart("sin(",1)
    {
        _about = "sin(X)";
        _usecase = "Sines of X";
    }
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        return sin(*(opnd));
    };
};

class Cosines: public CompoundStart {
public:
    Cosines() : CompoundStart("cos(", 1)
    {
        _about = "cos(X)";
        _usecase = "Cosines of X";
    };

    long double Exec(const long double opnd[], int opndcount, string str)  const override
    {
        return cos(*(opnd));
    };
};

class Tangents: public CompoundStart {
public:
    Tangents() : CompoundStart("tan(",1)
    {
        _about = "tan(X)";
        _usecase = "Tangents of X";
    };
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        long double csn = cos(*opnd);
        if ( abs(csn) < TooSmallNumber )
            throw string("Tangents: function is not defined for this angle.");
        else
            return tan(*(opnd));
    };
};


class Cotangents: public CompoundStart {
public:
    Cotangents() : CompoundStart("cotan(",1)
    {
        _about = "cotan(X)";
        _usecase = "Cotangents of X";
    };
    long double Exec(const long double opnd[], int opndcount, string str)  const override {
        long double ssn = sin(*opnd);
        if ( abs(ssn) < TooSmallNumber )
            throw string("Cotangents: function is not defined for this angle.");
        else
            return cos(*(opnd))/ssn;
    };
};

//Logarithmic
//*****************************************************

class Exponent: public Constant {
public:
    Exponent() : Constant("exp")
    {
        _about= "exp";
        _usecase = "Exponent = 2.71828182845904523536...";
    }
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        return 2.71828182845904523536028747135266249;
        }
    };


class NaturalLogarithm: public CompoundStart {
public:
    NaturalLogarithm() : CompoundStart("ln(", 1)
    {
        _about = "ln(X)";
        _usecase = "Natural logarithm of X (exponent is the base)";
    }
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        if ( (*opnd) < 0 )
            throw string("Logarithm: function is not defined for negatives.");
        else
            return log(*opnd);
    };
};

class Logarithm: public CompoundStart {
public:
    Logarithm() : CompoundStart("log(",2)
    {
        _about = "log(X; base)";
        _usecase = "Logarithm of X. Ex: log(8, 2) = 3";
    }
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        if ( (*opnd) < 0 )
            throw string("Logarithm: function is not defined for negatives.");
        else {
            long double znam = log(*(opnd+1));
            if (znam > TooSmallNumber)
                return log(*opnd)/znam;
            else
                throw string("Logarithm: function is not defined for this value.");
        }
    };
};

class Log10 : public CompoundStart {
public:
    Log10() : CompoundStart("lg(", 1) 
    {
        _about = "lg(X)";
        _usecase = "Binary logarithm of X (2 is the base). Ex: lg(8) = 3";
    }
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        if ((*opnd) < 0)
            throw string("Logarithm: function is not defined for negatives.");
        else {
            return log(*opnd) / log(2);
        }
    };
};


class Minimum : public CompoundStart {
public:
    Minimum() : CompoundStart("min(", 0) 
    {
        _about = "min(X1; X2;...Xn)";
        _usecase = "Minimum of any given numbers";
    }
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        int i = 0;
        long double min = *(opnd + i);
        for (i = 1; i < opndcount; i++) {
            if (*(opnd + i) < min) min = (*(opnd + i));
        }
        return min;
    };
};

class Maximum : public CompoundStart {
public:
    Maximum() : CompoundStart("max(", 0)
    {
        _about = "max(X1; X2;...Xn)";
        _usecase = "Maximum of any given numbers";
    }

    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        long double max = *(opnd);
        for (int i = 1; i < opndcount; i++) {
            if (*(opnd + i) > max) max = (*(opnd + i));
        }
        return max;
    };
};

class ArithmeticMean: public CompoundStart {
public:
    ArithmeticMean() : CompoundStart  ("arithmean(",0)
    {
        _about = "arithmean(X1; X2;...Xn)";
        _usecase = "Arithmetic mean of given numbers";
    }
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        long double sum = 0;
        for ( int i=0; i<opndcount; i++) {
            sum += *(opnd+i);
        }
        return sum /= opndcount;
   };
};

class GeometricMean : public CompoundStart {
public:
    GeometricMean() : CompoundStart("geomean(", 0)
    {
        _about = "geomean(X1; X2;...Xn)";
        _usecase = "Geometric mean of given numbers";
    }
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        long double res = 0;
        for (int i = 0; i < opndcount; i++) {
            res *= *(opnd + i);
        }
        return pow(res,1/opndcount);
    };
};


class HarmonicMean : public CompoundStart {
public:
    HarmonicMean() : CompoundStart("harmean(", 0)
    {
        _about = "harmean(X1; X2;...Xn)";
        _usecase = "Harmonic mean of given numbers";
    }
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        long double res = 0;
        for (int i = 0; i < opndcount; ++i) {
            res += 1 / (*(opnd + i));
        }
        return opndcount / res;
    };
};

class Median: public CompoundStart {
public:
    Median() : CompoundStart("median(", 0)
    {
        _about = "median(X1; X2;...Xn)";
        _usecase = "Median of given numbers";
    }
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        vector<long double> vec;
        for (size_t i = 0; i < opndcount; i++)  vec.push_back(*(opnd + i));
        sort(vec.begin(), vec.end());
        auto vs = vec.size();
        return opndcount % 2 ? vec[vs / 2] : (vec[vs / 2 - 1] + vec[vs / 2]) / 2;
    };
};

class Variance: public CompoundStart {
public:
    Variance() : CompoundStart("var(", 0)
    {
        _about = "var(X1; X2;...Xn)";
        _usecase = "Variance of given numbers";
    }
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        long double average{ 0 }, result{ 0 };
        for (size_t i = 0; i < opndcount; i++) 
            average += *(opnd + i);
        average /= opndcount;
        for (size_t i = 0; i < opndcount; i++)
            result += pow((*(opnd + i)) - average,2);
        result /= opndcount;
        return result;
    };
};


class ArithmeticProgression: public CompoundStart {
public:
    ArithmeticProgression() : CompoundStart("arithprog(",3) 
    {
        _about = "arithprog(base; ratio; elem_count)";
        _usecase = "Arithmetic progression. Ex: arithprog(10; 1; 3) = 33";
    }
    long double Exec(const long double opnd[], int opndcount, string str)  const override 
    {
        if (*(opnd+2) <= 0 )
            throw string("Arithmetic progression: Number of elements cannot be negative or zero.");
        return *(opnd+2) * (2* (*opnd) + *(opnd+1)*(*(opnd+2)-1))/2;
    };
};



class GeometricProgression : public CompoundStart {
public:
    GeometricProgression() : CompoundStart("geoprog(", 3)
    {
        _about = "geoprog(base; ratio; elem_count)";
        _usecase = "Geometric progression. Ex: geoprog(8; 1/2; 3) = 8 + 4 + 2 = 14";
    }
    long double Exec(const long double opnd[], int opndcount, string str)  const override
    {
        if (round(*(opnd + 2)) < 0)
            throw string("Geometric progression: Number of elements can't be negative.");
        if (round(*(opnd + 2)) == 0 && (*(opnd + 1) >= 1))
            throw string("Geometric progression: Common ratio (second opnd) must be < 1.");

        long double oper3 = round(*(opnd + 2));
        if ((*(opnd + 1)) == 1) // если это не прогрессия
            return (*opnd) * oper3;  //то вернуть колво членов умнож-е на первоый член
        else if ((!oper3) && (*(opnd + 1)) < 1) //это убывающая прогр-сия и нужна вся сумма
            return *opnd / (1 - *(opnd + 1));
        else //обычный случай
            return (*(opnd)) * (1 - pow(*(opnd + 1), oper3)) / (1 - *(opnd + 1));
    };
};








#endif