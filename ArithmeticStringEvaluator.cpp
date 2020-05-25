/* ArithmeticStringEvaluator.cpp // Compiled and tested with g++ (Ubuntu 5.4.0-6ubuntu1~16.04.10) 5.4.0 20160609
*  Author: Aaron Peressini
*  -------------------------------
*  This file contains the ArithmeticStringEvaluator class declaration and definition. ArithmeticStringEvaluator takes a string representing
*  a mathmetical expression as input, either from standard input via cin or from a text file in which each line is a separate 
*  expression, and evaluates the expression using a Recursive Descent Parser algorithm before displaying the result to standard
*  output via cout. This file also contains a main function that creates an instance of ArithmeticStringEvaluator and calls its run()
*  method to start the input parsing/evaluation.
*
*  The program takes either zero or one command line arguments, not including the program name. If zero arguments, the ArithmeticStringEvaluator
*  will run with cin as the input stream, waiting for the user to enter mathematical expressions to the console followed by a
*  carriage return. If there is one command-line argument, the argument should be text representing the name of a text file in
*  the same directory as the program executable. The program will then create an ifstream object and attempt to open the file.
*  If successful, each line of the file will be read as a separate expression and its result will be displayed to the console via cout.
*  If unsuccessful, an error message will be displayed and the program will exit.
*
*  ArithmeticStringEvaluator recognizes the following characters in order of precedence:
*       ()  : grouping
*       ^   : exponent
*       -   : unary negative
*       *   : multiplcation,  / : division
*       +   : addition     ,  - : subtraction
*
*  as well as any real number that can fit in type double, [2.22507e-308, 1.79769e+308], according to std::numeric_limits<double>::min() and max()
*       
*  For the ArtihmeticStringEvaluator run loop to end and the program to exit, the user can enter "quit" as input into cin, "quit"
*  can be read as a line from a file, or the input stream can simply reach the end of the text file.
*
*  To test the evaluator with random input from file, define macro TEST as 1. This will create an output stream with the file name
*  set to the command-line argument file name and call the generateRandomInput() function to generate pseudo-random expression strings
*  that will be inserted into the file and then be read back into the program and evaluated. WARNING: Please be aware that testing 
*  WILL overwrite your input file.
*
*  ---------------------------------
*  Resources: https://leetcode.com/problems/basic-calculator-ii/discuss/63140/recursive-descent-parser-in-c
*             https://www.youtube.com/watch?v=xUxBBWZt8uQ
*             http://www.cplusplus.com/
*/

#define  TEST  0

#include <iostream>
#include <string>
#include <list>
#include <algorithm>
#include <locale>
#include <cmath>
#include <fstream>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::list;
using std::isdigit;
using std::istream;
using std::ifstream;
using std::ofstream;
using std::pow;

const string unbalancedParensExceptionString = "Syntax Error: Parentheses are unbalanced";
const string invalidInputExceptionString     = "Syntax Error: Invalid input";
const string divideByZeroExceptionString     = "Computational Error: Divide by zero";

class ArithmeticStringEvaluator {

    private:
    double       result;        //holds result of each expression; 
    bool         active;        //while true, run loop is active; 
    list<string> tokens;        //contains string tokens used in mathematical expressions eg. +, -, *, /, etc.
    string       currentToken;  //holds the current string token
    istream&     inputStream;   //stream object that reads input from console or file

    void   displayResult();
    void   calculate(const string&);
    void   tokenizeInput(const string&);
    void   nextToken();
    double expr();
    double term();
    double prim();    

    public:
    ArithmeticStringEvaluator();
    ArithmeticStringEvaluator(istream&);
    void   run();  
    
};

//Default constructor; Calls overloaded constructor with std::cin as istream argument
ArithmeticStringEvaluator::ArithmeticStringEvaluator() : ArithmeticStringEvaluator(cin) {
    
}

//Overloaded constructor; Takes one argument of type istream that acts as the input stream for artithmetic expressions
ArithmeticStringEvaluator::ArithmeticStringEvaluator(istream& stream) : inputStream{stream} {
    
    result = 0.0;
    active = false;
    currentToken = "";
}

//run() method starts the run loop to read lines from inputStream
void ArithmeticStringEvaluator::run(){
    
    active = true;
        
    string input;
    cout << "input: ";
    
    while(active && getline(inputStream, input)){

        //if reading from file, display the current expression to be evaluated
        if(&inputStream != &cin){
            cout << input << endl;
        }
        
        if(input.length() > 0){
            transform(input.begin(), input.end(), input.begin(), ::tolower); 
            
            if(input.compare("quit") == 0) {
                active = false;
                cout << "User quit. " << endl;
            }
            
            else {
                try{
                    calculate(input);
                    if(!tokens.empty()){
                        if(currentToken == "(" || currentToken == ")"){
                            std::invalid_argument ia(unbalancedParensExceptionString);
                            throw ia;
                        }
                        else{
                            std::invalid_argument ia(invalidInputExceptionString);
                            throw ia;
                        }
                    }

                    displayResult();
                }
                catch(const std::exception& e){
                    cout << e.what() << endl << endl;
                }
                
                result = 0;
                tokens.clear();
                currentToken = "";
                cout << "input: ";
            }
        }
        else{
            displayResult();
            cout << "input: ";
        }
    }
    
    if(active){
        cout << "End of input file reached." << endl;;
    }
    
    cout << "Goodbye!" << endl;
}

//displayResult() prints the result of the current mathematical expression to the console via cout
void ArithmeticStringEvaluator::displayResult(){
    
    cout << "output: " << result << endl << endl;
}

//calculate(const string&) takes the current expression string as an argument, passes it to tokenizeInput() to be tokenized,
//then begins the Recursive Descent Parser algorithm by calling expr()
void ArithmeticStringEvaluator::calculate(const string& input){
    
    tokenizeInput(input);
    result = expr();
}

//tokenizeInput(const string&) takes the current expression string as input and logically separtes it into distinct 
//mathematical tokens to be evaluated
void ArithmeticStringEvaluator::tokenizeInput(const string& input){

    string num("");
    bool decimalPointUsed = false;
    char ch;
    for(int i = 0; i < input.length(); i++){
        ch = input.at(i);
        if(isdigit(ch) || (ch == '.' && !decimalPointUsed)){
            num += ch;
            if(ch == '.'){
                decimalPointUsed = true;
            }
        }
        else{
            if(!num.empty()){
                tokens.push_back(num);
                num.clear();
                decimalPointUsed = false;
            }
            if(ch != ' '){
                string tok("");
                tok += ch;
                tokens.push_back(tok);
            }
        }
    }
    
    if(!num.empty()){
        tokens.push_back(num);
        num.clear();
    }
    
    currentToken = tokens.front();
}

//nextToken() increments the currentToken parameter to the next token in the tokens list
void ArithmeticStringEvaluator::nextToken(){

    tokens.pop_front();
    if(!tokens.empty()){
        currentToken = tokens.front();
    }
    else{
        currentToken = "";
    }
}

//expr() is the first level method of the Recursive Descent Parser algorithm
double ArithmeticStringEvaluator::expr(){
    double res = term();
    while(currentToken == "+" || currentToken == "-"){
        if(currentToken == "+"){
            nextToken();
            res += term();
        }
        if(currentToken == "-"){
            nextToken();
            res -= term();
        }
    }

    return res;
}

//term() is the second level method of the Recursive Descent Parser algorithm
double ArithmeticStringEvaluator::term(){
    
    double res = prim();
    while(currentToken == "*" || currentToken == "/"){
        if(currentToken == "*"){
            nextToken();
            res *= prim();
        }
        if(currentToken == "/"){
            nextToken();
            double denom = prim();
            if(denom == 0.0){
                std::logic_error le(divideByZeroExceptionString);
                throw le;
            }
            res /= denom;
        }
    }
    return res;
}

//prim() is the third and base level method of the Recursive Descent Parser algorithm
double ArithmeticStringEvaluator::prim(){

    double res;

    if(currentToken == "("){
        nextToken();
        res = expr();
        if(currentToken != ")"){
            std::invalid_argument ia(unbalancedParensExceptionString);
            throw ia;
        }
        nextToken();
        if(currentToken == "^"){
            nextToken();
            res = pow(res, prim());
        }
        return res;
    }
    
    else if(currentToken == "-"){
        nextToken();
        res = -prim();
        return res;
    }

    else if(currentToken.length() > 0 && (isdigit(currentToken.at(0)) || currentToken.at(0) == '.')){
        res = strtod(currentToken.c_str(), NULL);
        nextToken();
        if(currentToken == "^"){
            nextToken();
            res = pow(res, prim());
        }
        return res;
    }
    
    else{
        std::invalid_argument ia(invalidInputExceptionString);
        throw ia;
    }
}

//generateRandomInput(ofstream&) function creates pseudo-random math expression strings and inserts them into the stream; used
//for testing
void generateRandomInput(ofstream& stream, int numExpr = 1000){

    char mathChars[18] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                            '(', ')', '^', '*', '/', '+', '-', '.'};

    srand(time(NULL));

    for(int i = 0; i < numExpr; i++){

        int randNum = (rand() % 10) + 1;
        string mStr("");

        for(int j = 0; j < randNum; j++){

            int randCh = rand() % 18;
            mStr += mathChars[randCh];
        }

        stream << mStr << endl;
    }

    stream.seekp(0);
}

int main(int argc, char ** argv){

    if(argc == 2){

    #if TEST
        ofstream ofs(argv[1], std::ios::trunc);
        if(ofs.is_open()){
            generateRandomInput(ofs);
        }
        ofs.close();
    #endif

        ifstream ifs(argv[1]);
        if(!ifs.is_open()){
            cout << "Problem with input file." << endl << "Please check file and try again." << endl;
        }
        else{
            ArithmeticStringEvaluator calc(ifs);
            calc.run();
        }

        ifs.close();
    }

    else{
        
        ArithmeticStringEvaluator calc;
        calc.run();
    }
    
    return 0;
}