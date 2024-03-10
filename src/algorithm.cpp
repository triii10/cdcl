#include "inc/algorithm.hpp"

state Algorithm::runAlgorithm(CLAUSE clauseList, LITERAL literalList, int clauseCount, int literalCount, TRAIL trail) {

    // Step 1. Simplify clauses by running exhaustive unit propagation
    CDCL c(clauseList, literalList, literalCount, clauseCount);
    c.findUnitClauses();
    // c.printClauseList();
    CLAUSE temp = c.exhaustiveUnitPropagation(originalClauseList);

    // We want to preserve the original clause list at the beginning of running the algorithm to get the implications
    if (originalClauseList.empty()) {
        originalClauseList = temp;
        originalLiteralList = c.getLiteralList();
    } 

    // Step 2. Get the current state after exhaustive UP
    state currentState;
    currentState = c.getCurrentState();

    // if current state is not UNDEF, then return the result
    if (currentState != UNDEF) {
        if (currentState == SAT)
            c.printCurrentModel();
        return currentState;
    }

    else {
        // Decide a variable
        int decisionLiteral = c.decide();
        c.addDecisionVariableToMap(decisionLiteral);

        // Add the decision variable to a new instance of clause list and
        // run the algorithm with that.

        CLAUSE tempClauseList = clauseList;
        clauseInfo decisionClauseInfo;
        std::vector<int> tempVector(literalCount+1, 0);
        tempVector[abs(decisionLiteral)] = decisionLiteral/abs(decisionLiteral);
        decisionClauseInfo.clause = tempVector;
        decisionClauseInfo.unit = true;
        decisionClauseInfo.unitLiteral = decisionLiteral;
        tempClauseList[clauseCount+1] = decisionClauseInfo;

        state newCurrentState = runAlgorithm(tempClauseList, literalList, clauseCount+1, literalCount);
        
        // Now check if the current state is SAT or UNSAT.
        // If it is Satisfiable, then return satisfiable
        if (newCurrentState == SAT)
            return newCurrentState;
            
        // If it is unsatisfiable, then we flip the decision variable, and run the algorithm again.
        // FOR CDCL -- if it is unsat, and the current decision level is the one to flip, then flip it and return result

        // To add some if condition to check backjumping level, below code remains same
        tempClauseList = clauseList;
        tempVector[abs(decisionLiteral)] = (-1*decisionLiteral)/abs(decisionLiteral);
        decisionClauseInfo.clause = tempVector;
        decisionClauseInfo.unitLiteral = -1 * decisionLiteral;
        tempClauseList[clauseCount+1] = decisionClauseInfo;
        return runAlgorithm(tempClauseList, literalList, clauseCount+1, literalCount);

        // If not backjump level, return to previous level
        // To add some code to implement that
    }

    return currentState;
}

void Algorithm::printResult(state& currentState) {

    std::cout << std::endl << std::endl << "-- RESULT --";
    if (currentState == SAT)
        std::cout << std::endl << "SATISFIABLE" << std::endl;
    else if (currentState == UNSAT)
        std::cout << std::endl << "UNSATISFIABLE" << std::endl;
    else 
        std::cout << std::endl << "UNDEFINED" << std::endl;
}

std::vector<int> Algorithm::getImpliedByClause(int unitLiteral, int clauseNo) {
    std::vector<int> tempClause = originalClauseList[clauseNo].clause;
    tempClause[abs(unitLiteral)] = 0;

    return tempClause;
}