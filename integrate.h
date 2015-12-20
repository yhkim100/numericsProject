#ifndef INTEGRATE_H
#define INTEGRATE_H


typedef std::map<std::string, Function*> myMap;
typedef std::pair<std::string, double> varPairs;
typedef std::pair<double,double> limitPairs;


bool parseIntBounds(Function* F, std::string::iterator &it, std::vector<limitPairs>& limitVector);
double getDoubleForIntegration(std::string::iterator &it);
bool storeIntegrationBounds(std::string::iterator &it, std::vector<limitPairs>& limitVector);
//void evaluateIntegration(std::vector<limitPairs>& limitVector, std::vector<std::vector<double> >& midpointTable, Function* F);








#endif
