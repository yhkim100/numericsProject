//header file for monteCarolo

#ifndef MONTE_H
#define MONTE_H

bool isMonte(std::string& line);
double doMonteCarlo(Function* F, std::string::iterator &it);
bool parseMCBounds(Function*F, std::string::iterator& it, std::vector<limitPairs>& limitVector);
double getNumTrials(std::string::iterator &it);
double getRandomBetween(double low, double high);




#endif
