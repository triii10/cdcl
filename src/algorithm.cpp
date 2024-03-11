#include "inc/algorithm.hpp"

resultStruct Algorithm::runAlgorithm(CLAUSE clauseList, LITERAL literalList, int clauseCount, int literalCount, int currentDecisionLevel, std::unordered_map<int, int> previousDecisionList, std::vector <trailInfo> previousTrailInfo) {

    resultStruct result = {.currentState = UNDEF, .conflictClause = {}, .backjumpLevel = 0};

    // Step 1. Simplify clauses by running exhaustive unit propagation
    CDCL c(clauseList, literalList, literalCount, clauseCount, previousDecisionList, previousTrailInfo);
    c.findUnitClauses();
    // Set the decision level
    c.setCurrentDecisionLevel(currentDecisionLevel);
    if (currentDecisionLevel == 0) {
        c.printClauseList();
        c.printLiteralList();
    }

    CLAUSE newClauseList = c.exhaustiveUnitPropagation(originalClauseList);

    // We want to preserve the original clause list at the beginning of running the algorithm to get the implications
    if (originalClauseList.empty()) {
        originalClauseList = newClauseList;
        originalLiteralList = c.getLiteralList();
    } 

    // Step 2. Get the current state after exhaustive UP
    result.currentState = c.getCurrentState();

    // if current state is not UNDEF, then return the result
    if (result.currentState != UNDEF) {
        if (result.currentState == SAT) {
            c.printCurrentModel();
        }

        if (currentDecisionLevel && result.currentState == UNSAT) {
            // Need to find the conflict clause and return UNSAT to the correct backjump level
            // First find conflict clause
            //  //  What do we need for conflict clause? 
                    // 1. The unit clause which caused the conflict. 
                    // 2. impledBy for unit literal and negation of unit literal
            // Second, get backjump level
            // Third, return three things, {state, conflict clause, backjump level}

            std::vector <int> conflictClause = c.findConflictClause(originalClauseList);

            // Get the backjump level from the conflictClause
            // Backjump level will be the max decision level of the literals in conflict clause
            int maxDecisionLevel = c.getBackjumpLevel(conflictClause, c.getCurrentDecisionLevel());
            std::cout << std::endl << "Backjump level: " << maxDecisionLevel << std::endl;

            result.backjumpLevel = maxDecisionLevel;
            result.conflictClause = conflictClause;
        }
        return result;
    }

    else {
        // Decide a variable
        int decisionLiteral = c.decide();

        // Add the decision variable to a new instance of clause list and
        // run the algorithm with that.

        CLAUSE tempClauseList = newClauseList;
        std::vector<int> tempVector(literalCount+1, 0);
        tempVector[abs(decisionLiteral)] = decisionLiteral/abs(decisionLiteral);
        tempClauseList[clauseCount + 1] = {.unit = true, .unitLiteral = decisionLiteral, .clause = tempVector};

        resultStruct newCurrentState = runAlgorithm(tempClauseList, literalList, clauseCount + 1, literalCount, currentDecisionLevel + 1, c.getLiteralDecisionLevelList(), c.getTrailInfo());

        // Now check if the current state is SAT or UNSAT.
        // If it is Satisfiable, then return satisfiable
        // if (newCurrentState.currentState == SAT)
        //     return newCurrentState;

        // Backjump will be implemented here.
        // If UNSAT and this is the backjump level, then add the conflict clause and run algorithm again with the same decision.
        if (newCurrentState.currentState == UNSAT && newCurrentState.backjumpLevel == currentDecisionLevel + 1) {
            tempVector[abs(decisionLiteral)] = -decisionLiteral/abs(decisionLiteral);
            tempClauseList[clauseCount + 1] = {.unit = false, .unitLiteral = 0,.clause = newCurrentState.conflictClause};
            tempClauseList[clauseCount + 2] = {.unit = true, .unitLiteral = -decisionLiteral, .clause = tempVector};
            originalClauseList[clauseCount + 1] = tempClauseList[clauseCount + 1];
            return runAlgorithm(tempClauseList, literalList, clauseCount + 2, literalCount, currentDecisionLevel + 1, c.getLiteralDecisionLevelList(), c.getTrailInfo());
        }

        else 
            return newCurrentState;

        // To add some if condition to check backjumping level, below code remains same

        // decisionLiteral = -decisionLiteral;
        // std::cout << std::endl << "** (" << currentDecisionLevel << ")" << " FLIPPING DECIDE --> " << decisionLiteral << std::endl;
        // tempClauseList = newClauseList;
        // tempVector[abs(decisionLiteral)] = decisionLiteral/abs(decisionLiteral);
        // tempClauseList[clauseCount + 1] = {.clause = tempVector, .unit = true, .unitLiteral = decisionLiteral};
        // // This is not decision, but assertion, so decision level will not change.
        // return runAlgorithm(tempClauseList, literalList, clauseCount+1, literalCount, currentDecisionLevel, c.getLiteralDecisionLevelList(), c.getTrailInfo());

        // If not backjump level, return to previous level
        // To add some code to implement that
    }

    return result;
}

void Algorithm::printResult(state& currentState) {

    std::cout << std::endl << std::endl << "-- RESULT --";
    if (currentState == SAT)
        std::cout << std::endl << "SATISFIABLE" << std::endl;
    else if (currentState == UNSAT)
        std::cout << std::endl << "UNSATISFIABLE" << std::endl;
    else 
        std::cout << std::endl << "UNKNOWN" << std::endl;
}

std::vector<int> Algorithm::getImpliedByClause(int unitLiteral, int clauseNo) {
    std::vector<int> tempClause = originalClauseList[clauseNo].clause;
    tempClause[abs(unitLiteral)] = 0;

    return tempClause;
}