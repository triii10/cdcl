#include <iostream>
#include <sstream>
#include "structure.hpp"

class Dimacs {
    
    private:
    int countVar, countClauses;
    std::vector< std::vector <int> > rawInputList;

    public:
    void Input(CLAUSE&, LITERAL&);
    void Output();
    int getVarCount();
    int getClauseCount();
    std::vector <std::vector <int> > getClauseList();
};