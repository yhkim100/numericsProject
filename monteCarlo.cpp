//File for monte Carlo stuff

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include "numerics.h"
#include "tree.h"
#include "integrate.h"
#include "monteCarlo.h"
#include <time.h>

typedef std::map<std::string, Function*> myMap;
typedef std::pair<std::string, double> varPairs;
typedef std::pair<double,double> limitPairs;

bool isMonte(std::string& line){
  if(line.find("mcint") != std::string::npos){
    std::string::iterator it = line.begin();
    skipSpace(it);
    std::string op = getNextStringForIntegration(it);
    if(op.compare("mcint") == 0){
      return 1;
    }
    else{
      return 0;
    }
  }
  else{
    return 0;
  }
}

double doMonteCarlo(Function* F, std::string::iterator& it){
  //std::cout<<"SUCCESS"<<'\n';
  //Get number of trials
  double numTrials = getNumTrials(it);
  //std::cout<<"Number of trials "<<numTrials<<'\n';
  if(isnan(numTrials)){
    throw std::string("Invalid Number of Trials");
  }
  //Find Lower and Upper Limits and store into vector
    std::vector<limitPairs> limitVector;
    if( storeIntegrationBounds(it, limitVector)){
      //std::cout<<"Compiled Correctly"<<'\n';
      //get random variables to evaluate
      std::vector<double> inputRandomVal(limitVector.size(),0);
      srand(time(NULL));
      double MCnum = 0;
      double max = 0;
      double min = 0;
      double rando = 0;
    
      for(double i = 0; i<numTrials; i++){
       for(unsigned j = 0; j<limitVector.size(); j++){
        //get random number and store into inputRandomVal vector
        //std::cout<<"LOW BOUND IS "<<limitVector[j].first<<'\n';
        //std::cout<<"UPPER BOUND IS "<<limitVector[j].second<<'\n';
        rando = getRandomBetween(limitVector[j].first,limitVector[j].second);
        //std::cout<<"Storing Random Value of "<<rando<<'\n';
        inputRandomVal[j] = rando;
       }
      //RUN EVALUATE TO GET NUMBER
      F->setVars(inputRandomVal);
      double Fout = F->evaluate();
      
      if(Fout > max){
        max = Fout;
      }
      if(Fout < min){
        min = Fout;
      }

      //DO MC RATIO
      double testRand = getRandomBetween(min, max);
        if(Fout>0 && (Fout>testRand && testRand > 0)){
          MCnum++;
        }
        else if(Fout<0 && (Fout<testRand && testRand <0)){
          MCnum--;
        }

      }//FINISHED EVALUATING AND GETTING RATIO
    //std::cout<<"MonteCarlo Ratio "<<MCnum<<'\n';
    //NEED TO GET AREA AND COMPUTE ACTUAL INTEGRATION RESULT
    double outputBounds= max-min;
    double areaOfIntegration = 0;
    double inputBounds = 1;
    //Get Input Bounds
    for(unsigned i = 0; i<limitVector.size(); i++){
      inputBounds = inputBounds * (limitVector[i].second - limitVector[i].first);
    }
    areaOfIntegration = inputBounds * outputBounds;
    double finalResult = (MCnum/(double)numTrials) * areaOfIntegration;
    //std::cout<<"AREA OF INTEGRATION: "<<areaOfIntegration<<'\n';
    return finalResult;
    }
    else{
      throw std::string("ERROR");
    }
}

double getNumTrials(std::string::iterator &it){
  std::string str = getNextStringForIntegration(it);
  char* endptr;
  double num = strtod(str.c_str(), &endptr);
  if((str.find('.')!=std::string::npos)){
    return NAN;
  }
  if( ((*endptr == ' ') || (*endptr == '\0')) ){
    ++it;
    return num;
  }
  else{
    return NAN;
  }
}


double getRandomBetween(double low, double high){
  double result = low + (double)(rand()) / (RAND_MAX/(high-low));
  return result;
}
