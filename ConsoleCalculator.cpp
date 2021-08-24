
#include <iostream>
#include <memory>
#include "Calculator.h"


void DelJunkSymbols(string& s) {
    static char todel[] = { ' ','\n','\r','\v','\f','\t' };
    string::size_type pos = string::npos;
    for (char i : todel) {
        pos = s.find(i);
        while (pos != string::npos) {
            s.erase(pos, 1);
            pos = s.find(i);
        }
    }
};

Calculator& Init()
{
    auto Calc = new Calculator();
    //base
    Calc->AddNodeType(*new BinaryNumber());
    Calc->AddNodeType(*new HexNumber());
    Calc->AddNodeType(*new Number());
    Calc->AddNodeType(*new CompoundEnd(")"));
    Calc->AddNodeType(*new CompoundStart("(", 0));
    Calc->AddNodeType(*new OperandDelimiter(";"));
    //arithmetic
    Calc->AddNodeType(*new UnaryMinus());
    Calc->AddNodeType(*new Addition());
    Calc->AddNodeType(*new Substraction());
    Calc->AddNodeType(*new Multiplication());
    Calc->AddNodeType(*new Division());
    Calc->AddNodeType(*new Pow());
    Calc->AddNodeType(*new Abs());
    Calc->AddNodeType(*new Round());
    Calc->AddNodeType(*new Trunc());
    //arithmetic advanced
    Calc->AddNodeType(*new Factorial());
    Calc->AddNodeType(*new ArithmeticProgression());
    Calc->AddNodeType(*new GeometricProgression());
    //Bit
    Calc->AddNodeType(*new BitAND());
    Calc->AddNodeType(*new BitOR());
    Calc->AddNodeType(*new BitXOR());
    Calc->AddNodeType(*new BitNOT());
    //statistic
    Calc->AddNodeType(*new Minimum());
    Calc->AddNodeType(*new Maximum());
    Calc->AddNodeType(*new Variance());
    Calc->AddNodeType(*new Median());
    Calc->AddNodeType(*new ArithmeticMean());
    Calc->AddNodeType(*new GeometricMean());
    Calc->AddNodeType(*new HarmonicMean());
    //trigonometric
    Calc->AddNodeType(*new Sines());
    Calc->AddNodeType(*new Cosines());
    Calc->AddNodeType(*new Tangents());
    Calc->AddNodeType(*new Cotangents());
    //logarithmic
    Calc->AddNodeType(*new Logarithm());
    Calc->AddNodeType(*new NaturalLogarithm());
    Calc->AddNodeType(*new Log10());
    //constants
    Calc->AddNodeType(*new Exponent());
    Calc->AddNodeType(*new Pi());

    Calc->ShowHelp();

    return *Calc;
}

static const char* prompt = "KissThePig > ";


int main()
{
    auto calc = make_unique<Calculator>(Init());
    string rpn;
   
    while (true) {
        try {
            cout << prompt;
            getline(std::cin, rpn);
            DelJunkSymbols(rpn);
            if (rpn == "exit" || rpn == "quit") return 0;
            //cout << rpn << endl;
            calc->ToReversePolishNotation(rpn);
            cout << calc->Execute() << endl;
        }
        catch (string s)
        {
            cout << "   " << s << endl;
        }
        catch (...) 
        {
           cout << "   " << "Unknown error. Try again.\n";
        }
        
    }
    return 0;
}

