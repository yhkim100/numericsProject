#ifndef _TREE_H
#define _TREE_H


#include <sstream>
#include <iostream>
#include <ctype.h>
#include <cstdlib>
#include <string.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <map>
#include <cassert>

typedef std::pair<std::string, double> varPairs;

class Expression{
 protected:
  double num;
 public:
  virtual std::string toString() const = 0;
  virtual double evaluate() = 0;
  virtual double evaluate(std::vector<varPairs>& temp) const = 0;
  virtual ~Expression(void){}
};

class Function: public Expression{
  private: 
    std::vector<varPairs> vars;
    Expression * expr;
  public:
    Function(std::vector<varPairs> temp):vars(temp),expr(NULL){}
    std::vector<varPairs> getVectorOfVarPairs(){
      //std::cout<<"Returning vars vector<varPairs> of size"<<vars.size()<<'\n';
      return vars;
    }
    void printVariables(){
      std::vector<varPairs>::iterator it = vars.begin();
      std::cout<<"PRINTING VARIABLES"<<'\n';
      while( it != vars.end()){
        std::cout<<(*it).first<<'\n';
        ++it;
      }
    }
    std::vector<varPairs> getFunctionVector(){
      return vars;
    }
    int getArgSize(){
      return vars.size();
    }
    virtual std::string toString() const{
    return expr->toString();
    }
    virtual double evaluate(std::vector<varPairs>& temp) const {
      return expr->evaluate(temp);
    }

    virtual double evaluate() {
      return expr->evaluate(vars);
    }
    void setExpression(Expression* temp){
      expr=temp;
    }
    
    void setVars(std::vector<double>& temp){
    //  assert(temp.size() == vars.size());
      std::vector<double>::iterator tempIt = temp.begin();
      std::vector<varPairs>::iterator varsIt = vars.begin();
      while((tempIt != temp.end()) && (varsIt != vars.end())){
          (*varsIt).second = *tempIt;
          ++varsIt;
          ++tempIt;
      }
    }

    bool isVar (std::string str){
      std::vector<varPairs>::iterator it = vars.begin();
      //std::cout<<"checking if "<< str<< " is valid variable"<<'\n';
      while(it != vars.end()){
        if(str.compare((*it).first) == 0){
          return 1;
        }
        ++it;
      }
      return 0;
    }

    virtual ~Function(){
    delete expr;
    }
};



typedef std::map<std::string, Function*> myMap;

class CallExpression: public Expression{
  private:  
    std::string name;
    Function* F;
    std::vector<Expression*> args;
  public:
    CallExpression(std::string fname, Function* F):name(fname),F(F){}
    int getArgSize(){
      return args.size();
    }
    void addArgs(Expression* expr){
      args.push_back(expr);
    }
    void getName(){
      std::cout<<name<<'\n';
    }
    void printVariables(){
      return F->printVariables();
    }
    virtual std::string toString() const{
      std::stringstream ss;
      //std::cout<<"size of args vector is "<<args.size()<<'\n';
      if(args.size()==0){
        return F->toString();
      }
      ss<<name<<'(';
      for(unsigned int i = 0; i<args.size(); ++i){
        ss<<args[i]->toString();
        if(i<args.size()-1){
          ss<<", ";
        }
      }
      ss<<')';
      return ss.str();
    }
    virtual double evaluate(std::vector<varPairs> & temp) const{
    //call exp, f-> setvars, num = f->evaluate, return num
//      F->setVars(temp);
 /*     std::vector<varPairs> passingArg;
      std::vector<double> resultsNumbers;
      std::vector<Expression*>::iterator it = args.begin();
      std::vector<varPairs>::iterator vIt = temp.begin();
      while(it!=args.end()){
       //PASS IN THE VARIABLE INTO passingArg
       double result = (*it)->evaluate(temp);
       passingArg.push_back(std::make_pair((*vIt).first,result));
       resultsNumbers.push_back(result);
       ++it;
       ++vIt;
      }
      F->setVars(resultsNumbers);
      return F->evaluate(passingArg);*/
      std::vector<varPairs> passingArgs = F->getVectorOfVarPairs();
      std::vector<Expression*>::const_iterator exprIt = args.begin();
      std::vector<varPairs>::iterator argsIt = passingArgs.begin();
      while(exprIt!=args.end()){
        double result = (*exprIt)->evaluate(temp);
        (*argsIt).second = result;
        ++argsIt;
        ++exprIt;
      }
      //std::cout<<"Call expression evaluate to "<<F->evaluate(passingArgs)<<'\n';
      return F->evaluate(passingArgs);
    }


    virtual double evaluate() {
      return 0;
    }

    virtual ~CallExpression(){
    std::vector<Expression*>::iterator it = args.begin();
    while(it!=args.end()){
      delete *it;
      ++it;
        }
    }
};


class VarExpression: public Expression{
  private:
  std::string var;
  public:
  VarExpression(std::string input){
    this->var = input;
  }
  std:: string toString() const{
    return var;
  }
  double evaluate(std::vector<varPairs>& temp) const{
    //iterate and substitute
    std::vector<varPairs>::iterator it = temp.begin();
    while(it != temp.end()){
      if(var.compare((*it).first) == 0){
        return (*it).second;
      }
      ++it;
    }
    return NAN; //should never happen
  }

  virtual double evaluate() {
     return 0;
  }

  virtual ~VarExpression(){}
};

class NumExpression: public Expression{
 public:
 NumExpression(double input){
    this->num = input;
  }

  std::string toString() const{
    std::stringstream ss;
    ss << num;
    return ss.str();
  }
  double evaluate(std::vector<varPairs>& temp) const{
    return this->num;
  }

  virtual double evaluate() {
     return 0;
  }

  virtual ~NumExpression(){}
};

class PlusExpression: public Expression{
 private:
  Expression * lhsExp;
  Expression * rhsExp;
 public:
  PlusExpression(Expression * lhs, Expression * rhs){
    lhsExp = lhs;
    rhsExp = rhs;
  }
  std::string toString() const{
    std::stringstream ss;
    //ss << "test plus";
    ss << "(" << lhsExp->toString() << " + " << rhsExp->toString() << ")";
    return ss.str();
  }
  double evaluate(std::vector<varPairs>& temp) const{
    return lhsExp->evaluate(temp)+ rhsExp->evaluate(temp);
    //    num = (lhsExp->getNum()) + (rhsExp->getNum());
    //return this->num + (lhsExp->getNum()); //+ (rhsExp->getNum());
  }

  virtual double evaluate() {
     return 0;
  }

  virtual ~PlusExpression(){
    delete lhsExp;
    delete rhsExp;
  }
};

class MinusExpression: public Expression{
 private:
  Expression * lhsExp;
  Expression * rhsExp;
 public:
  MinusExpression(Expression * lhs, Expression * rhs){
    lhsExp = lhs;
    rhsExp = rhs;
  }
  std::string toString() const{
    std::stringstream ss;
    //ss << "test minus";
    ss << "(" << lhsExp->toString() << " - " << rhsExp->toString() << ")";
    return ss.str();
  }

  double evaluate() {
     return 0;
  }

  double evaluate(std::vector<varPairs>& temp) const{    
    return (lhsExp->evaluate(temp)) - (rhsExp->evaluate(temp));
  }
  virtual ~MinusExpression(){
    delete lhsExp;
    delete rhsExp;
  }
};


class TimesExpression: public Expression{
 private:
  Expression * lhsExp;
  Expression * rhsExp;
 public:
  TimesExpression(Expression * lhs, Expression * rhs){
    lhsExp = lhs;
    rhsExp = rhs;
  }
  std::string toString() const{
    std::stringstream ss;
    ss << "(" << lhsExp->toString() << " * " << rhsExp->toString() << ")";
    return ss.str();
  }

  double evaluate() {
     return 0;
  }

  double evaluate(std::vector<varPairs>& temp) const{
    return (lhsExp->evaluate(temp)) * (rhsExp->evaluate(temp));
  }
  virtual ~TimesExpression(){
    delete lhsExp;
    delete rhsExp;
  }
};



class DivExpression: public Expression{
 private:
  Expression * lhsExp;
  Expression * rhsExp;
 public:
  DivExpression(Expression * lhs, Expression * rhs){
    lhsExp = lhs;
    rhsExp = rhs;
  }
  std::string toString() const{
    std::stringstream ss;
    ss << "(" << lhsExp->toString() << " / " << rhsExp->toString() << ")";
    return ss.str();
  }

  double evaluate() {
     return 0;
  }

  double evaluate(std::vector<varPairs>& temp) const{
    return lhsExp->evaluate(temp) / rhsExp->evaluate(temp);
  }
  virtual ~DivExpression(){
    delete lhsExp;
    delete rhsExp;
  }
};


class PowExpression: public Expression{
 private:
  Expression * lhsExp;
  Expression * rhsExp;
 public:
  PowExpression(Expression * lhs, Expression * rhs){
    lhsExp = lhs;
    rhsExp = rhs;
  }
  std::string toString() const{
    std::stringstream ss;
    ss << "(" << lhsExp->toString() << " ^ " << rhsExp->toString() << ")";
    return ss.str();
  }

  double evaluate() {
     return 0;
  }

  double evaluate(std::vector<varPairs>& temp) const{
    return pow(lhsExp->evaluate(temp), rhsExp->evaluate(temp));
  }
  virtual ~PowExpression(){
    delete lhsExp;
    delete rhsExp;
  }
};


class SqrtExpression: public Expression{
 private:
  Expression * lhsExp;
  Expression * rhsExp;
 public:
  SqrtExpression(Expression * lhs, Expression * rhs){
    lhsExp = lhs;
    rhsExp = rhs;
  }
  std::string toString() const{
    std::stringstream ss;
    ss << "sqrt(" << lhsExp->toString() << ")";
    return ss.str();
  }

  double evaluate() {
     return 0;
  }

  double evaluate(std::vector<varPairs>& temp) const{
    return sqrt(lhsExp->evaluate(temp));
  }
  virtual ~SqrtExpression(){
    delete lhsExp;
    delete rhsExp;
  }
};


Expression * parse(std::string::iterator& it, myMap& functionMap,Function* temp);
void skipSpace(std::string::iterator& it);
Expression* makeExpr(std::string op, Expression* lhs, Expression* rhs);
Expression* parseOp(std::string::iterator& it, myMap& functionMap,Function* temp);

double parseEval(std::string::iterator &it, myMap& functionMap);
double computeFunction(std::string::iterator &it, myMap& functionMap, Function* F);

#endif

