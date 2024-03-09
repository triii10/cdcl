#include "main.hpp"

int main(int argc, char const *argv[])
{
    Dimacs A;
    A.Input();
    // A.Output();
    
    CDCL c(A.getClauseList(), A.getVarCount(), A.getClauseCount());
    c.findUnitClauses();
    c.printClauseList();
    std::unordered_map< int, clauseInfo > temp = c.unitPropagation();
    

    // Check if there is a conflict after UP, then it is UNSAT
    state currentState;
    c.isConflictPresent(currentState);
    std::cout << std::endl << "UNSTAISFIABLE? " << (currentState ? "No" : "Yes") << std::endl;
    c.printClauseList();
    c.printLiteralList();

    return 0;
}
