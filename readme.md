    ================================  
          Console Calculator              
    ================================     


CONTENTS                                    
--------------------------------------  
1. For Users                              
2. For Developers (in Russian)  
--------------------------------------  

1. For Users
--------------------------------------

Console *** Kiss the Pig *** Calculator
----------------------------------------
Supports arithmetic, trigonometric, logarithmic, some statistic functions.  
  0b101110                              Binary number, without fraction part  
  0x1AF233C                             Hex number, without fraction part  
  pow(X, Y)                             Pow(X, Y). Ex: pow(2, 3) = 8  
  abs(X)                                Absolule value of X  
  round(X)                              Rounds X to nearest integer number  
  trunc(X)                              Eliminates fractional part of X  
  !X                                    Factorial of X  
  arithprog(base; ratio; elem_count)    Arithmetic progression. Ex: arithprog(10; 1; 3) = 33  
  geoprog(base; ratio; elem_count)      Geometric progression. Ex: geoprog(8; 1/2; 3) = 14  
  X & Y                                 Bit AND  
  X | Y                                 Bit OR  
  X ^ Y                                 Bit XOR  
  ~ X                                   Bit NOT  
  min(X1; X2;...Xn)                     Minimum of any given numbers  
  max(X1; X2;...Xn)                     Maximum of any given numbers  
  var(X1; X2;...Xn)                     Variance of given numbers  
  median(X1; X2;...Xn)                  Median of given numbers  
  arithmean(X1; X2;...Xn)               Arithmetic mean of given numbers  
  geomean(X1; X2;...Xn)                 Geometric mean of given numbers  
  harmean(X1; X2;...Xn)                 Harmonic mean of given numbers  
  sin(X)                                Sines of X  
  cos(X)                                Cosines of X  
  tan(X)                                Tangents of X  
  cotan(X)                              Cotangents of X  
  log(X; base)                          Logarithm of X. Ex: log(8, 2) = 3  
  ln(X)                                 Natural logarithm of X (exponent is the base)  
  lg(X)                                 Binary logarithm of X (2 is the base). Ex: lg(8) = 3  
  exp                                   Exponent = 2.71828182845904523536...   
  pi                                    Pi = 3.14159265358979...  
Operands inside function calls must be delimited by ;  
You can build complex expressions like:  
  
   ((2.2212+3*10) * max(10; 33+2^arithmean(12; ln(11); 5*6.23; 3^7); 11)) / (13 * 12)  
  
    
2. For Developers  
--------------------------------------   
Calculator can be easily extended. New functions can be added in minutes.  
Learn Russian and read comments in source files!  

