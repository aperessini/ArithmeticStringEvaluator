ArithmeticStringEvaluator.cpp // Compiled and tested with g++ (Ubuntu 5.4.0-6ubuntu1~16.04.10) 5.4.0 20160609
Author: Aaron Peressini
-------------------------------
This file contains the ArithmeticStringEvaluator class declaration and definition. ArithmeticStringEvaluator takes a string representing a mathematical expression as input, either from standard input via cin or from a text file in which each line is a separate expression, and evaluates the expression using a Recursive Descent Parser algorithm before displaying the result to standard output via cout. This file also contains a main function that creates an instance of ArithmeticStringEvaluator and calls its run()method to start the input parsing/evaluation.


The program takes either zero or one command line arguments, not including the program name. If zero arguments, the ArithmeticStringEvaluator will run with cin as the input stream, waiting for the user to enter mathematical expressions to the console followed by a carriage return. If there is one command-line argument, the argument should be text representing the name of a text file in the same directory as the program executable. The program will then create an ifstream object and attempt to open the file. If successful, each line of the file will be read as a separate expression and its result will be displayed to the console via cout. If unsuccessful, an error message will be displayed and the program will exit.


ArithmeticStringEvaluator recognizes the following characters in order of precedence:
       ()  : grouping
       ^   : exponent
       -   : unary negative
       *   : multiplication,  / : division
       +   : addition     ,  - : subtraction


as well as any real number that can fit in type double, [2.22507e-308, 1.79769e+308], according to std::numeric_limits<double>::min() and max()
       
For the ArtihmeticStringEvaluator run loop to end and the program to exit, the user can enter "quit" as input into cin, "quit" can be read as a line from a file, or the input stream can simply reach the end of the text file.


To test the evaluator with random input from a file, define macro TEST as 1. This will create an output stream with the file name set to the command-line argument file name and call the generateRandomInput() function to generate pseudo-random expression strings that will be inserted into the file and then be read back into the program and evaluated. WARNING: Please be aware that testing WILL overwrite your input file.
---------------------------------
Resources:
https://leetcode.com/problems/basic-calculator-ii/discuss/63140/recursive-descent-parser-in-c
https://www.youtube.com/watch?v=xUxBBWZt8uQ
http://www.cplusplus.com/