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
#include "gradient.h"

typedef std::map<std::string, Function*> myMap;
typedef std::pair<std::string, double> varPairs;
typedef std::pair<double,double> limitPairs;

int main(int argc, char** argv){
std::ifstream ifs;

myMap functionMap;

//check for correct number of arguments

if(argc==1){
  std::cout<<"please specify file to read"<<'\n';
  return EXIT_FAILURE;
}
for(signed i = 1; i!= argc; i++){
ifs.open(argv[i]);
std::string line;
std::cout<<"============="<<'\n';
std::cout<<"START READING"<<'\n';
std::cout<<"============="<<'\n';

while(std::getline (ifs,line)){
  if(!parenthesisMatch(line)){
    std::cout<<line<<" <- parenthesis mismatch"<<'\n';
    continue;
  }
  if(isDefine(line)){
    std::string FALSE = "FALSE";
    std::string defineSide = findIDs(line);
    if(!defineSide.compare(FALSE)){
//      std::cout<<line<<" <- unable to read variables"<<'\n';
      continue;
    }
    std::cout<<line<<" <- define line read"<<'\n';
    //std::cout<<defineSide<<" <- returned ID list " <<'\n';
    std::string expressionSide = findRHS(line);
    //std::cout<<expressionSide<<" <- expression"<<'\n';
    //NEED TO BUILD THE TREE HERE
    std::vector<varPairs> temp;
    getAndStoreVariables(defineSide, temp);

    Function* myFunction = new Function(temp); 
  //Start with Parse Expression and then Parse OP
    std::string::iterator exprIt = expressionSide.begin();
    Expression* functionExpr = parse(exprIt, functionMap, myFunction);
    if(functionExpr == NULL){
      std::cout<<"Unable to parse"<<'\n';
      delete myFunction;
      continue;
    }
    myFunction->setExpression(functionExpr);
   // std::cout<<myFunction->toString()<<" <- Calling toString()"<<'\n';
    std::string funcName = getFunctionName(defineSide);
   // std::cout<<funcName<<" <- function name"<<'\n';


    if(functionMap.insert(std::pair<std::string, Function*> (funcName,myFunction)).second == true){
      std::cout<<"defined "<<defineSide<<'\n';
      std::cout<<defineSide<<" = "<<myFunction->toString()<<'\n';
//      std::cout<<"storing this function "<<funcName<<'\n';
//      std::cout<<"args size is "<<myFunction->getArgSize()<<'\n';
    }
    else{
      delete myFunction;
    }
  }

  if(isEvaluate(line)){
    std::cout<<line<<" <- contains evaluate" <<'\n';
    // GET THE ENTIRE EXPRESSION AFTER EVALUTE KEYWORD
    std::string evaluateCmd = line.substr(line.find_first_of('('));
    //std::cout<<line<<'\n';
    //std::cout<<evaluateCmd<<'\n';
    std::string::iterator it = evaluateCmd.begin();
    double result = parseEval(it, functionMap);
    if(isnan(result)){
      std::cout<<"Received NAN for one of the arguments, failed to evaluate"<<'\n';
      std::cout<<"Failed to "<<line<<'\n';
    }
    else{
      std::cout<<evaluateCmd<<" = " <<result<<'\n';
    }
  }
  if(isNumint(line)){
    std::cout<<line<<" <-is numint command line" <<'\n';
    /*
      1)  get function name and find in map
          if not found, INVALID
      2)  if found: get and store stepsize (make sure this is valid)
      3)  continue getting min and max limit arguments 

    */

    std::vector<limitPairs> limitVector;
    std::string::iterator it = line.begin();
    skipSpace(it); //basically removes leading whitespace if any
    std::string numintCmd = getNextStringForIntegration(it);
    //std::cout<<numintCmd<<" <- should say numint"<<'\n';
    skipSpace(it);
    std::string Fname = getNextStringForIntegration(it);
    //std::cout<<Fname<<" <- should be function name"<<'\n';
    myMap::iterator mapIt = functionMap.find(Fname);
      if(mapIt == functionMap.end()){
        std::cout<<"Unable to find "<<Fname<<" in map"<<'\n';
        continue;
      }
      /*
        ATTEMPTS TO PARSE INTEGRATION LINE
        IF SUCCESS: PRINT RESULT LINE
        ELSE: MOVE ON TO NEXT LINE
      */
      else{
      std::cout<<"ATTMEPTING TO PARSE BOUNDS"<<'\n';
        if(parseIntBounds((*mapIt).second, it, limitVector)){
          //std::cout<<"COMPILED CORRECTLY"<<'\n';
        }
        else{
          std::cout<<"MOVING TO NEXT LINE OF FILE"<<'\n';
          limitVector.clear();
          continue;
        }
      }
  }
  if(isMonte(line)){
    //DO MONTECARLO INTEGRATION
    std::cout<<"Monte Carlo detected in line "<<line<<'\n';
    //Find matching function in map
    std::string::iterator it = line.begin();
    skipSpace(it);
    std::string monteCarloCmd = getNextStringForIntegration(it);
    skipSpace(it);
    std::string Fname = getNextStringForIntegration(it);
    myMap::iterator mapIt = functionMap.find(Fname);
      if(mapIt == functionMap.end()){
        std::cout<<"Unable to find "<<Fname<<" in map"<<'\n';
        continue;
      }
      else{
        //std::cout<<"Found function, attempting MonteCarlo Integration"<<'\n';
        try{
            double result = doMonteCarlo((*mapIt).second, it);
            std::cout<<"MonteCarlo Integration resulted in "<<result<<'\n';
        }
        catch(std::string& e){
          std::cout<<e<<'\n';
        }

      }
  }
  if(isMax(line) || isMin(line)){
    std::cout<<"Gradient detected in line "<<line<<'\n';
    //Find matching function in map
    std::string::iterator it = line.begin();
    skipSpace(it);
    std::string Cmd = getNextStringForIntegration(it);
    skipSpace(it);
    std::string Fname = getNextStringForIntegration(it);
    myMap::iterator mapIt = functionMap.find(Fname);
      if(mapIt == functionMap.end()){
        std::cout<<"Unable to find "<<Fname<<" in map"<<'\n';
        continue;
      }
      else{//DO GRADIENT COMPUTATION
        skipSpace(it);
        double gamma = getDouble(it);
        if(isnan(gamma) || (gamma <= 0)){
        std::cout<<"Invalid Gamma"<<'\n';
        continue;
        }
        skipSpace(it);
        double convergeDist = getDouble(it);
        std::cout<<"Converge Distance of "<<convergeDist<<'\n';
        if(isnan(convergeDist) || convergeDist<= 0){
        std::cout<<"Invalid Converge Distance"<<'\n';
        }
        else{
        //std::cout<<"GETTING STARTPOINTS"<<'\n';
          skipSpace(it);
          std::vector<double> startpoints;
          try{
            startpoints = getStartPoints((*mapIt).second, it);
        
          //DETERMINE MIN OR MAX
          //std::cout<<"COMPUTING MIN OR MAX"<<'\n';
            if(Cmd.compare("max") == 0){
            //std::cout<<"DOING GRAD ASCENT"<<'\n';
            //std::cout<<"Converge Dist is "<<convergeDist<<'\n';
              double result = doGradAscent(gamma,convergeDist,(*mapIt).second,startpoints);
              std::cout<<"For Function "<<(*mapIt).first<<" = "<<(*mapIt).second->toString()<<'\n';
              std::cout<<"a maximum of "<<result<<" was found "<<'\n';
              std::cout<<"using a gamma value of "<<gamma<<" and a convergene distance of "<<convergeDist<<'\n';
              //std::cout<<"Found Maximum of "<<result<<'\n';
            }
            if(Cmd.compare("min") == 0){
            //std::cout<<"DOING GRAD DESCENT"<<'\n';
            //std::cout<<"Converge Dist is "<<convergeDist<<'\n';
              double result = doGradDescent(gamma,convergeDist,(*mapIt).second,startpoints);
              std::cout<<"For Function "<<(*mapIt).first<<" = "<<(*mapIt).second->toString()<<'\n';
              std::cout<<"a minimum of "<<result<<" was found "<<'\n';
              std::cout<<"using a gamma value of "<<gamma<<" and a convergene distance of "<<convergeDist<<'\n';
              //std::cout<<"Found Minimum of "<<result<<'\n';
            }
          }
          catch(...){
            std::cout<<"Failed to do Gradient"<<'\n';
          }
        }
      }
  }
}//Out of while getline loop

std::cout<<"============"<<'\n';
std::cout<<"DONE READING"<<'\n';
std::cout<<"============"<<'\n';
ifs.close();
}
//CLEANUP MAP
myMap::iterator it = functionMap.begin();
while(it!=functionMap.end()){
  delete (*it).second;
  ++it;
}

return EXIT_SUCCESS;

}



std::string getNextString(std::string& line){
  removeLeadingWhitespace(line);
  //find the next whitespace location, create substring to that location, remove any parenthesis if needed
  std::size_t length = line.find_first_of(' ');
  std::string result = line.substr(0,length);
  result.erase(remove(result.begin(),result.end(),'('),result.end());
  result.erase(remove(result.begin(),result.end(),')'),result.end());
  return result;
}


bool isOnlyDouble(std::string& line){
  char *endptr;
  strtod(line.c_str(), &endptr);
  if(*endptr == '\0'){
    return 1;
  }
  return 0;
}


double getDouble(std::string::iterator &it){
  std::string str = getNextString(it);
  char* endptr;
  double num = strtod(str.c_str(), &endptr);
  if( (*endptr == ' ') || (*endptr == '\0') ){
    return num;
  }
  else
  return NAN;
}

double getDouble(std::string & str){
  char* endptr;
  double num = strtod(str.c_str(), &endptr);
  if( (*endptr == ' ') || (*endptr == '\0') ){
    return num;
  }
  else
    return NAN;
}

bool foundNum (std::string input){
  //std::cout<<"checking if "<<input<<" is a number"<<'\n';
  std::string::iterator it = input.begin();
  while(it != input.end()){
    if(isdigit(*it)){
      return 1;
    }
    ++it;
  }
  return 0;
}

bool parenthesisMatch (std::string line){
  if(std::count(line.begin(),line.end(),'(') == std::count(line.begin(),line.end(),')')){
    return 1;
  }
  else return 0;
}

bool isDefine(std::string line){
  std::size_t match = line.find("define");
  if(match!=std::string::npos){
    //line contains "define" but check if there is other nonsense involved
    //remove all whitespace
    std::size_t startOfIDs = line.find('(',0);
    std::string lhs = line.substr(0,startOfIDs);
    lhs.erase(remove_if(lhs.begin(),lhs.end(),isspace),lhs.end());
    if(lhs.compare("define") != 0){
//      std::cout<<line<<" <-- this line contains 'define' but is not formatted properly"<<'\n';
      return 0;
    }
//  std::cout<<line<<" <-- ATTEMPING TO DEFINE"<<'\n';    
  return 1;
  }
  else
//    std::cout<<line<<" <-- did not find define"<<'\n';
    return 0;
}


bool isEvaluate(std::string line){
  std::size_t match = line.find("evaluate");
  if(match!=std::string::npos){
  std::size_t startOfIDs = line.find('(',0);
  std::string lhs = line.substr(0,startOfIDs);
  lhs.erase(remove_if(lhs.begin(),lhs.end(),isspace),lhs.end());
    if(lhs.compare("evaluate") != 0){
      return 0;
    }
    else
      return 1;
    }
  else
    return 0;
}

std::string removeLeadingWhitespace(std::string& line){
  std::size_t strBegin = line.find_first_not_of(' ');
  std::string result = line.substr(strBegin);
  return result;
}

//line -> define (f x y z) = ( ... )
//GRABS TOTAL LIST OF ID's --> (f x y z)
std::string findIDs(std::string line){
    std::size_t startOfIDs = line.find_first_of('(',0);
    std::size_t endOfIDs = line.find_first_of('=',0);

  //ERROR CHECKING 
    if((startOfIDs == std::string::npos) || (endOfIDs == std::string::npos)){
      return "FALSE"; 
      } 
   // std::cout<<line<<"<-- line"<<'\n';
  int openPar = std::count(line.begin(),line.end(),'(');
  int closePar = std::count(line.begin(),line.end(),')');
 // std::cout<<"( count: "<<openPar<<'\n'<<") count: " << closePar<<'\n';
  //CHECK IF PARENTHESIS MATCH UP
    if(openPar == closePar){  
    //do Define functions
    //std::cout<<line<<" <- contains define"<<'\n';
    //search for the first open parenthesis and remove the define portion

    //substr( position, length of string)
    std::size_t lengthOfIDs = endOfIDs-startOfIDs;
    std::string listOfIDs = line.substr (startOfIDs, lengthOfIDs);
//  std::cout<<listOfIDs<<'\n';
    return listOfIDs; 

    }
    else{
    return "FALSE";
    }

}


std::string findRHS(std::string& line){
    std::size_t startOfRHS = line.find('=',line.find_first_of(')',0));
    std::string RHS = line.substr (startOfRHS+1);
    std::string result = removeLeadingWhitespace(RHS);
    return result;
}

void getAndStoreValues(std::string lhs, std::vector<double>& storage){

    std::string temp;
    //make a loop to grab each individual for left and right hand side
    //loop for each individual ID on left hand side
    //grab the first character, iterate to next white space, grab next character, repeat until begin == end
    lhs = removeLeadingWhitespace(lhs);
    lhs.erase(0,lhs.find_first_of(' '));
    while(lhs.size()>1){ //while not npos
      lhs=removeLeadingWhitespace(lhs);
     // std::cout<<lhs<<" <- current list"<<'\n';
      temp = getNextString(lhs);
      char* endptr;
      double result = strtod(temp.c_str(),&endptr);
      storage.push_back(result);
      if(lhs.find_first_of(' ')!=std::string::npos){
      lhs = lhs.erase(0,lhs.find_first_of(' '));
      }
      else{
      break;
      }
    }
}

void getAndStoreVariables(std::string lhs, std::vector<varPairs>& storage){

    std::string temp;
    //make a loop to grab each individual for left and right hand side
    //loop for each individual ID on left hand side
    //grab the first character, iterate to next white space, grab next character, repeat until begin == end
    lhs = removeLeadingWhitespace(lhs);
    lhs.erase(0,lhs.find_first_of(' '));
    while(lhs.size()>1){ //while not npos
      lhs=removeLeadingWhitespace(lhs);
     // std::cout<<lhs<<" <- current list"<<'\n';
      temp = getNextString(lhs);
      storage.push_back(make_pair(temp,9999));
      if(lhs.find_first_of(' ')!=std::string::npos){
      lhs = lhs.erase(0,lhs.find_first_of(' '));
      }
      else{
      break;
      }
    }
}

std::string getFunctionName(std::string lhs){
  lhs=removeLeadingWhitespace(lhs);
  std::string result = getNextString(lhs);
  return result;
}

std::string doubleToString (double input){
  std::ostringstream strs;
  strs << input;
  std::string str = strs.str();
  return str;
}

std::string getNextString(std::string::iterator &it){
  std::string result;
  result.clear();
  while(*it!= ' '){
    if( (*it == ')') || (*it == '\0')){
      return result;
    }
    result+=*it;
    ++it;
  }
  return result;
}

std::string getNextStringForIntegration(std::string::iterator &it){
  skipSpace(it);
  std::string result;
  result.clear();
  while(*it!= ' '){
    if(*(it+1) == '\0'){
      return result+=*it;
    }
    else{
    result+=*it;
    ++it;
    }
  }
  return result;
}

bool isNumint(std::string &line){
  if(line.find("numint") != std::string::npos){
    std::string::iterator it = line.begin();
    skipSpace(it);
    std::string op = getNextStringForIntegration(it);
    if(op.compare("numint") == 0){
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
