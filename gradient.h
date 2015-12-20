#ifndef _GRADIENT_H
#define _GRADIENT_H

bool isMin(std::string &line);
bool isMax(std::string &line);
std::vector<double> getStartPoints(Function* F, std::string::iterator& it);
double doGradAscent(double gamma, double convergeDist, Function* F, std::vector<double>& startpoints);
double doGradDescent(double gamma, double convergeDist, Function* F, std::vector<double>& startpoints);
double derivative(Function* F, std::vector<double>& args, unsigned i);
double getGradientVector(Function* F, std::vector<double>& args);
void multAll (std::vector<double>& poitns, double scale);
std::vector<double> sumAll (std::vector<double>& vect1, std::vector<double>& vect2);
std::vector<double> subtrAll (std::vector<double>& vect1, std::vector<double>& vect2);

#endif
