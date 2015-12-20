//FILE FOR GRADIENT STUFF
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
#include <cctype>


bool isMin(std::string &line){
  if(line.find("min") != std::string::npos){
    std::string::iterator it = line.begin();
    skipSpace(it);
    std::string op = getNextStringForIntegration(it);
    if(op.compare("min") == 0){
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

bool isMax(std::string &line){
  if(line.find("max") != std::string::npos){
    std::string::iterator it = line.begin();
    skipSpace(it);
    std::string op = getNextStringForIntegration(it);
    if(op.compare("max") == 0){
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

std::vector<double> getStartPoints(Function* F, std::string::iterator& it){
  std::vector<double> arguments;
  while(*it != '\0'){
  //std::cout<<"TRYING TO GET START POINTS"<<'\n';
    double num = getDouble(it);
    if(isnan(num)){
      std::cout<<num<<" is not a valid start point"<<'\n';
      throw std::string ("Num is not a valid number");
    }
    arguments.push_back(num);
    skipSpace(it);
  }
    if((unsigned)F->getArgSize() != (unsigned)arguments.size()){
      std::cout<<"Argument Mismatch"<<'\n';
      throw std::string ("Agrument Mismatch");
    }
  return arguments;
}

double derivative(Function* F, std::vector<double>& args, unsigned i){
  double h = 0.000000001;
  std::vector<double> evalArgs = args;
  evalArgs[i] += h;
  F->setVars(evalArgs);
  double f1 = F->evaluate();
  F->setVars(args);
  double f2 = F->evaluate();
  return (f1-f2)/h;
}

std::vector<double> getGradientVector(Function* F, std::vector<double>& arguments){
  std::vector<double> temp;
  for(unsigned i = 0; i<arguments.size(); i++){
    temp.push_back(derivative(F, arguments, i));
  }
return temp;
}

double distanceForm (std::vector<double>& points){
  double distance = 0;
  double distanceSquared = 0;
  std::vector<double> temp (points.size(), 0);
  for(unsigned i = 0; i<points.size(); i++){
    temp[i] = points[i]*points[i];
  }
  for (unsigned x = 0; x<temp.size(); x++){
    distanceSquared = distanceSquared + temp[x];
  }
  distance = sqrt(distanceSquared);
  return distance;
}

void multAll (std::vector<double>& points, double scale){
double temp = 0;
  for(unsigned i = 0; i<points.size(); i++){
    temp = points[i]; 
    points[i] = temp * scale;
  }
}

std::vector<double> sumAll (std::vector<double>& vect1, std::vector<double>& vect2){
  std::vector<double> result (vect1.size(), 0);
  for(unsigned i = 0; i<vect1.size(); i++){
    result[i] = vect1[i] + vect2[i];
  }
  return result;
}

std::vector<double> subtrAll (std::vector<double>& vect1, std::vector<double>& vect2){
  std::vector<double> result (vect1.size(), 0);
  for(unsigned i = 0; i<vect1.size(); i++){
    result[i] = vect1[i] - vect2[i];
  }
  return result;
}

double doGradAscent(double gamma, double convergeDist, Function* F, std::vector<double>& startpoints){
  std::vector<double> pnew(startpoints.size(), 0);
  std::vector<double> step = getGradientVector(F, startpoints);
  while(distanceForm(step) > convergeDist){
  //std::cout<<"Distance is currently "<<distanceForm(step)<<'\n';
    multAll(step, gamma); //gamma * gradient
    pnew = sumAll(startpoints, step);
    startpoints = pnew;
    step = getGradientVector(F, startpoints);
  }
  F ->setVars(pnew);
  return F->evaluate();
}

double doGradDescent(double gamma, double convergeDist, Function* F, std::vector<double>& startpoints){
  std::vector<double> pnew(startpoints.size(), 0);
  std::vector<double> step = getGradientVector(F, startpoints);
  while(distanceForm(step) > convergeDist){
  //std::cout<<"Distance is currently "<<distanceForm(step)<<'\n';
    multAll(step, gamma); //gamma * gradient
    pnew = subtrAll(startpoints, step);
    startpoints = pnew;
    step = getGradientVector(F, startpoints);
  }
  F ->setVars(pnew);
  return F->evaluate();
}



