#ifndef _NUMERICS_H
#define _NUMERICS_H

#include <algorithm>
#include <map>
#include <locale>
#include <stdexcept>
#include <sstream>
#include <cmath>
#include <vector>

typedef std::pair<std::string, double> varPairs;  


std::string getNextString(std::string& line);
double getDouble(std::string::iterator& it);
double getDouble(std::string& str);
bool foundNum(std::string input);
bool parenthesisMatch (std::string line);
bool isDefine(std::string line);
bool isEvaluate(std::string line);
bool isNumint(std::string &line);
std::string removeLeadingWhitespace(std::string& line);
std::string findIDs(std::string line);
std::string findRHS(std::string& line);
void getAndStoreValues(std::string lhs, std::vector<double>& storage);
void getAndStoreVariables(std::string lhs, std::vector<varPairs>& storage);
std::string getFunctionName(std::string lhs);
std::string doubleToString (double input);
std::string getNextString(std::string::iterator &it);
std::string getNextStringForIntegration(std::string::iterator &it);
bool isOnlyDouble(std::string& line);

#endif
