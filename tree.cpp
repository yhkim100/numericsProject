#include <string>
#include <iostream>
#include <ctype.h>
#include <cstdlib>
#include <stdio.h>
#include <map>
#include <cmath>
#include "numerics.h"
#include "tree.h"


typedef std::map<std::string, Function*> myMap;
typedef std::pair<std::string, double> varPairs;

Expression * parse(std::string::iterator& it, myMap& functionMap,Function* temp);

void skipSpace(std::string::iterator& it) {
  while(isspace(*it)) {
    ++it;
  }
}

Expression * makeExpr(std::string op, Expression * lhs, Expression * rhs) {

  if(op.compare("+") == 0){
    return new PlusExpression(lhs,rhs);
    }
  if(op.compare("-") == 0){
    return new MinusExpression(lhs,rhs);
    }
  if(op.compare("*") == 0){
    return new TimesExpression(lhs,rhs);
    }
  if(op.compare("/") == 0){
    return new DivExpression(lhs,rhs);
    }
  if(op.compare("sqrt") == 0){
    return new SqrtExpression(lhs,rhs);
    }
  if(op.compare("pow") == 0){
    return new PowExpression(lhs,rhs);
    }
    std::cerr << "Impossible op char: " << op << "\n";
    abort();
}

bool isValidOp(std::string Op) {
  if(Op.compare("+") == 0){
    return 1;
  }
  if(Op.compare("-") == 0){
    return 1;
  }
  if(Op.compare("*") == 0){
    return 1;
  }
  if(Op.compare("/") == 0){
    return 1;
  }
  if(Op.compare("pow") == 0){
    return 1;
  }
  if(Op.compare("sqrt") == 0){
    return 1;
  }
  return 0;
}


Expression * parseOp(std::string::iterator& it, myMap& functionMap, Function* temp) { //const char ** strp
  skipSpace(it);

  //CHECK IF Op is FUNCTION NAME in MAP
  //if true: create Call Expression 
  std::string Fname;
  Fname.clear();
  while(*it != ' '){
    if(*it == ')'){
      break;
    }
  Fname+=*it;
  ++it;
  }
  //std::cout<<Fname<<" <- FUNCTION NAME SEARCHING" <<'\n';
  myMap::iterator mapIt = functionMap.find(Fname);
  if (mapIt!=functionMap.end()){
  //std::cout<<" FOUND A MATCHING FUNCTION"<<'\n';
     CallExpression* myCallExpr = new CallExpression(Fname, &(*(mapIt->second)));
      while(*it!= ')'){
        //std::cout<<"THIS IS THE NAME OF FUNCTION FOUND"<<'\n';
        //myCallExpr->getName();
     //   myCallExpr->printVariables();
     //   std::cout<<myCallExpr->getArgSize()<<'\n';
        Expression* argument = parse(it, functionMap, temp);
        //std::cout<<argument<<" adding to args"<<'\n';
        if(argument!=NULL){
        myCallExpr->addArgs(argument);
        }
      }
      ++it;
      //std::cout<<"myCall args size is "<<myCallExpr->getArgSize()<<'\n';
      //std::cout<<"function's args size is "<<(*mapIt).second->getArgSize()<<'\n';
      if(myCallExpr->getArgSize() < (*mapIt).second->getArgSize()){
        delete myCallExpr;
        std::cout<<"ARGUMENT MISMATCH"<<'\n';
        return NULL;
      }
      else{
      //std::cout<<"made Call Expression "<<'\n';
      return myCallExpr;
     }
  }

  else if (!isValidOp(Fname)) {
    //check if is Var or is Number (a constant) else INVALID
    if(temp->isVar(Fname) && (*it==')')){
      return new VarExpression(Fname);
    }
    else{
     double num = getDouble(Fname); 
        //std::cout<<Fname<< " returns the number " << num << std::endl;
      if(isnan(num)){
        return NULL;
      }
      else{
        return new NumExpression(num);
      }
    }
  }

  if(temp->isVar(Fname) && (*it==')')){
    return new VarExpression(Fname);
  }

  std::string op = Fname; 
  //std::cout<<op<<" <- operator is this"<<'\n';
  ++it;
  Expression * rhs;
  Expression * lhs = parse(it,functionMap,temp);
  if (lhs == NULL) {
    return NULL;
  }


  //sqrt case
  if(op.compare("sqrt")==0){
    rhs = NULL;
  }  
  else{
    rhs = parse(it, functionMap,temp);
  if (rhs == NULL) {
    delete lhs;
    return NULL;
    }
  }
  skipSpace(it);
  //std::cout<<*it<<"<- should not be a space"<<'\n';
  if (*it == ')') {
    ++it;
    return makeExpr(op,lhs,rhs);
  }
  std::cerr <<"Expected ) but found " << *it << "\n";
  delete lhs;
  delete rhs;
  return NULL;
}

Expression * parse(std::string::iterator &it, myMap& functionMap, Function* temp) {
  skipSpace(it);
  //std::cout<<"parse checking this -> "<<*it<<'\n';
  if (*it == '\0') {
    std::cerr << "End of line found mid expression!\n";
    return NULL;
  }
  else if (*it == '(') {
    // (op E E)
    ++it;
    return parseOp(it, functionMap,temp);
  }
  else if(isdigit(*it) || *it=='-' || *it == '.'){
    //assuming number
    std::string::iterator holdPos = it;
    double num = getDouble(it);
    if(isnan(num)){
      std::string var = getNextString(holdPos);

      if(temp->isVar(var)){
        return new VarExpression(var);
      }
      else{
        std::cout<<var<<" <- this is invalid"<<'\n';
        return NULL;
      }
    }
    else{
      return new NumExpression(num);
    }
  }

  else{ //either variable or invalid
    skipSpace(it);
    if(*it == ')'){return NULL;}
    std::string var = getNextString(it);
    //std::cout<<"checking if "<<var<<" is variable or invalid"<<'\n';
      if(foundNum(var)){
        std::cout<<var<<" is invalid";
        return NULL;
      }
      //std::cout<<var<<" is valid"<<'\n';
      if(temp->isVar(var)){
        //std::cout<<var<<" is a valid variable"<<'\n';
        return new VarExpression(var);
      }
      else{
      return NULL;
      }
  }
}


double parseEval(std::string::iterator &it, myMap& functionMap){
  std::vector<double> tempValues;
  if(*it == '('){
    //get FuncName
    ++it;
    std::string FuncName = getNextString(it);
    myMap::iterator functionMapIt = functionMap.find(FuncName);
    if(functionMapIt != functionMap.end()){
     double returnVal = computeFunction(it, functionMap, (*functionMapIt).second);
     //std::cout<<"Return value of parseEval is "<<returnVal<<'\n';
     return returnVal;
    }
    else{
      std::cout<<"Could not find the function "<<FuncName<<" within the map"<<'\n';
      return NAN;
    }
  }
  else{
    std::cout<<"Failed to Parse"<<'\n';
    return NAN;
  }

}

double computeFunction(std::string::iterator &it, myMap& functionMap, Function* F){
  std::vector<double> tempValues;
  skipSpace(it);
  //std::cout<<"checking iterator at "<<*it<<'\n';
  while(*it != ')'){
    if(*it == '('){ //recursive portion of evaluate
      double result = parseEval(it, functionMap);
      ++it;
      skipSpace(it);
   //   std::cout<<"string iterator is now pointing at "<<*it<<" after getting result from parsing"<<'\n';
      if(isnan(result)){
   //     std::cout<<"computation result "<<result<<" is not of type double"<<'\n';
        return NAN;
      }
      else{
  //      std::cout<<"adding the result "<<result<<" to argument vector"<<'\n';
        tempValues.push_back(result);
  //      std::cout<<"argument vector is now of size "<<tempValues.size()<<'\n';
  //      std::cout<<"string iterator is now pointing at "<<*it<<'\n';
      }
    }
    else if( (isdigit(*it) || (*it == '-') || (*it == '.') )){
      std::string value = getNextString(it);
      skipSpace(it);
      if(isnan(getDouble(value))){
        std::cout<<"the value "<<value<<" is not valid double"<<'\n';
        return NAN; 
      }
      else{
   //     std::cout<<"adding the value "<<value<<" to argument vector"<<'\n';
        tempValues.push_back(getDouble(value));
   //     std::cout<<"argument vector is of size "<<tempValues.size()<<'\n';
      }
    }
    else{ //INVALID case
      std::cout<<*it<<" <- is not valid"<<'\n';
      return NAN;
    }
  }

 // std::cout<<"argument vector is update to be of size "<<tempValues.size()<<'\n';
  //sub in values to variables then evaluate
  if((unsigned)F->getArgSize() > (unsigned)tempValues.size()){
    std::cout<<"Function has argument mismatch, not all variables have a value assigned"<<'\n';
    //std::cout<<"Current Function has "<<tempValues.size()<<" values"<<'\n';
    //std::cout<<"Function to substitute into has "<<F->getArgSize()<<" variables"<<'\n';
    return NAN;
  }
  else{
    F->setVars(tempValues);
    //std::cout<<"Function Variables have been replaced"<<'\n';
    /*
    std::vector<varPairs> vars = F->getVectorOfVarPairs();
    //fill in values to varPair vector
    std::vector<double>::iterator tempIt = tempValues.begin();
    std::vector<varPairs>::iterator varsIt = vars.begin();
    while(varsIt != vars.end()){
      (*varsIt).second = *tempIt;
      ++varsIt;
      ++tempIt;
    }*/
  return F->evaluate();
  }
}

















