#include <stdint.h>
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

typedef std::map<std::string, Function*> myMap;
typedef std::pair<std::string, double> varPairs;
typedef std::pair<double,double> limitPairs;

bool parseIntBounds(Function* F, std::string::iterator &it, std::vector<limitPairs>& limitVector){
    
        skipSpace(it);
        double stepsize = getDouble(it);
        //std::cout<<"Checking stepsize"<<'\n';
        //make sure stepsize is valid double
        if(isnan(stepsize)){
          return 0;
        }
        else{
        //std::cout<<"stepsize of "<<stepsize<<'\n';
          //start getting min and max limits
          //std::cout<<"iterator points to "<<(*it==' ')<<'\n';
          skipSpace(it);
          if( !storeIntegrationBounds(it, limitVector)){
            return 0;
          }
          //std::cout<<"iterator points to "<<(*it==' ')<<'\n';
          //check size of function's arguments and vector size
          //make sure limitVector is completely filled
          if((unsigned)F->getArgSize() != limitVector.size()){
            std::cout<<"ARGUMENT MISMATCH, CANNOT INTEGRATE"<<'\n';
            return 0;
          }
          else{
            //std::vector<std::vector<double> >midpointTable = createIntegrationTable(F,stepsize,limitVector); 
            
            std::cout<<"WILL ATTEMPT TO INTEGRATE"<<'\n';
            std::vector<varPairs> args = F->getVectorOfVarPairs();
            //std::cout<<"Midpoint Table width is "<<args.size()<<'\n';
            double sizeTable = 1;
            std::vector<limitPairs>::iterator limitIt = limitVector.begin();
            //Calculate the height of table
            while(limitIt != limitVector.end()){
              double low = (*limitIt).first;
              double high = (*limitIt).second;
              double numRect = (high - low)/stepsize;
              //std::cout<<"Adding "<<numRect<<" to size"<<'\n';
              sizeTable = abs(sizeTable*numRect);
              //std::cout<<"Current size: "<<sizeTable<<'\n';
              ++limitIt;
            }
            //std::cout<<"size of Table "<<sizeTable<<'\n';
            //Create the table with sizeTable as height and args.size as width
            //std::cout<<"Creating table with size "<<sizeTable<<'\n';
            //std::vector<std::vector<double> > midpointTable (sizeTable, std::vector<double> (ceil(args.size()), 0));
            std::vector<double> numRectsTable (ceil(args.size()),0);
            std::vector<double> vecOfBases (ceil(args.size()), 0);
            std::vector<double> vecOfTemps (ceil(args.size()), 0);
            double numRectangle = 1;
            double result = 0;
            for(unsigned argptr = 0; argptr<args.size(); argptr++){
              //number of rectangles is (high-low)/stepsize
              //std::cout<<"NumRect is "<<numRectangle<<'\n';
                numRectangle = numRectangle * ((limitVector[argptr].second - limitVector[argptr].first)/stepsize);
                numRectsTable[argptr] = numRectangle;
                vecOfBases[argptr] = (limitVector[argptr].first + (limitVector[argptr].first + stepsize))/2;
                vecOfTemps[argptr] = vecOfBases[argptr]-stepsize;
            }

            //Set up Evaluate Vector 
            for(unsigned evalPtr = 0; evalPtr < sizeTable; evalPtr++){
              for(unsigned m = 0; m < limitVector.size(); m++){
               // if( (fmod(evalPtr, (sizeTable/numRectsTable[m]))) == 0){
               if( ((unsigned)evalPtr%((unsigned)sizeTable/(unsigned)numRectsTable[m]) == 0) ){
                  vecOfTemps[m]+=stepsize;
                  if(vecOfTemps[m] > limitVector[m].second){
                    vecOfTemps[m] = vecOfBases[m];
                  }
                }
              }
              //Do evaluate
              F->setVars(vecOfTemps);
              result = result + F->evaluate();
            }
            result = pow(stepsize, args.size())*result;
            std::cout<<"Integration Result: "<<result<<'\n';
            return 1; 
        }
      }
}


double getDoubleForIntegration(std::string::iterator &it){
  std::string str = getNextStringForIntegration(it);
  //std::cout<<"checking if "<<str<<" is a double"<<'\n';
  //if(*it == '\0'){return NAN;}
  char* endptr;
  double num = strtod(str.c_str(), &endptr);
  if( (*endptr == ' ') || (*endptr == '\0')){
    ++it;
    //std::cout<<"num is "<<num<<'\n';
    return num;
  }
  else{
    //std::cout<<"returning null\n";
    return NAN;
  }
}

bool storeIntegrationBounds(std::string::iterator &it, std::vector<limitPairs> &limitVector){
          while(*it!='\0'){
            //get lower then upper bound
            //
            double low = getDoubleForIntegration(it);
            //std::cout<<low<<" <- lower bound"<<'\n';
            if(isnan(low)){
              std::cout<<" lower bound is not a valid number\n";
              return 0;
            }
            if(*it == '\0'){
              std::cout<<" UPPER BOUND NOT SPECIFIED"<<'\n';
              return 0;
            }
            else{
              double high = getDoubleForIntegration(it);
              if(isnan(high)){
                std::cout<<" upper bound is not a valid number\n";
                return 0;
              }
              //std::cout<<high<<" <-upper bound\n";
              //At this point, both low and high are valid doubles, check if low<high
              if(low >= high){
                std::cout<<"Invalid Arguments, attempted to set "<<low<<" as lower bound and "<<high<<" as upper bound "<<'\n';
                return 0;
              }
              //std::cout<<"adding pair: "<<low<<", "<<high<<'\n';
              if(!isnan(low) && !isnan(high)){
                limitVector.push_back(limitPairs(low,high));
              }
              //std::cout<<"limitVector size: "<<limitVector.size()<<'\n';
              //std::cout<<"interator pointing at "<<*it<<'\n';
            }
          } //out of while
  return 1;
}
