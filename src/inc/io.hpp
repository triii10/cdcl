#include <iostream>
#include <sstream>
#include "structure.hpp"

class Dimacs {
    
    private:
    int countVar, countClauses;

    public:
    void Input();
    void Output();
    int getVarCount();
    int getClauseCount();
    std::vector <std::vector <int> > getClauseList();
};