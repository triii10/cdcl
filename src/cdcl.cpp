#include "cdcl.hpp"
#include <iostream>


std::ostream& operator<<(std::ostream& os, const CLAUSE& container) {
  os << "[";
  for(CLAUSE::const_iterator it = container.begin(); it != container.end(); it++) {
    os << "Clause No. : "<< it->first << std::endl;
    os << "Unit Clause? " << it->second.unit << std::endl;
    it->second.unit ? os << "Unit Literal: " << it->second.unitLiteral << std::endl : os << "Not a Unit clause" << std::endl;
    os << "Clause :" << it->second.clause << std::endl;
  }
  os << "]";
  return os;
}

CDCL::CDCL(CLAUSE& clauseListCopied, LITERAL& literalListCopied, int variableCount, int clauseCount) {

    clauseList = clauseListCopied;
    literalList = literalListCopied;

    model = std::vector <bool> (variableCount+1, false);
    literalCount = variableCount;
    this->clauseCount = clauseCount;
}

int CDCL::findUnitClauses() {
    // Idea is to parse through all clauses and see which has only one literal
    // Then update the index 0 in clauseList for that clause to the literal which is unit.

    bool anyUnitClauseExists = false;

    constructLiteralList();
    for (CLAUSE::const_iterator it = clauseList.begin(); it != clauseList.end(); it++) {
        bool hasUnitClause = false;
        int unitClauseNo = -1;
        for (int literalInClause = 1; literalInClause < it->second.clause.size(); literalInClause++) {
            if (it->second.clause[literalInClause]) {
                if (hasUnitClause) {
                    hasUnitClause = false;
                    unitClauseNo = -1;
                    break;
                }

                hasUnitClause = true;
                unitClauseNo = it->second.clause[literalInClause]*literalInClause;
            }
        }
        if (hasUnitClause) {
            clauseList[it->first].unitLiteral = unitClauseNo;
            clauseList[it->first].unit = true;
            anyUnitClauseExists = true;
        }
    }

    return anyUnitClauseExists;
}

void CDCL::printClauseList() {
    printClauseList(clauseList);
}

void CDCL::printClauseList(const CLAUSE& passedClauseList) {

    std::cout << std::endl << "------" << std::endl;
    for (CLAUSE::const_iterator it = passedClauseList.begin(); it != passedClauseList.end(); it++) {
        std::cout << "Clause " << it->first << (it->second.unit ? " (Unit)": "") << ": ";
        for (int j = 1; j < it->second.clause.size(); j++) {
            if (it->second.clause[j])
                std::cout << it->second.clause[j]*j << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "------" << std::endl;
}

void CDCL::printLiteralList() {

    std::cout << std::endl << "------" << std::endl;
    for (LITERAL::const_iterator it = literalList.begin(); it != literalList.end(); it++) {
        std::cout << "Literal " << it->first << " is present in clauses: ";
        for (int j = 0; j < it->second.size(); j++) {
            std::cout << it->second[j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << "------" << std::endl;
}

CLAUSE CDCL::exhaustiveUnitPropagation(CLAUSE& originalClauseList) {
    // Unit propagation will find unit clauses, and for each unit clause, it will go through the literalList
    // to find the clauses where the unit literal appears
    // Then, will apply subsumption elimination to remove the clauses, and remove negative occurances of the literal 

    CLAUSE& result = clauseList;
    while (findUnitClauses()) {
        int unitLiteral = 0;
        for (CLAUSE::const_iterator it = clauseList.begin(); it != clauseList.end(); it++) {
            clauseInfo tempClauseInfo = it->second;
            if (tempClauseInfo.unit) {
                unitLiteral = tempClauseInfo.unitLiteral;
                break;
            }
        }

        if (!unitLiteral)
            break;

        CLAUSE result = unitPropagation(unitLiteral, originalClauseList);
        if (getCurrentState() == UNSAT)
            return result;
    }

    return result;
}


CLAUSE CDCL::unitPropagation(int unitLiteral, CLAUSE& originalClauseList) {
    // Unit propagation will find unit clauses, and for each unit clause, it will go through the literalList
    // to find the clauses where the unit literal appears
    // Then, will apply subsumption elimination to remove the clauses, and remove negative occurances of the literal 

    CLAUSE& result = clauseList;
    std::vector<int> impledBy;

    if (!unitLiteral)
        return result;

    // First perform subsumption elimination of positive literal to remove clauses
    std::cout << std::endl << "** UP: " << unitLiteral << " --> ";
    model[abs(unitLiteral)] = (abs(unitLiteral) == unitLiteral);

    for (int clause: literalList[unitLiteral]) {
        // Do clause elimination
        std::cout << clause << " ";

        if (clauseList[clause].unit)
            impledBy = getImpliedByClause(unitLiteral, clause, originalClauseList);
        result.erase(clause);
    }

    std::cout << ", (-) ";
    for (int clause: literalList[-1 * unitLiteral]) {
        // Do negative literal elimination
        if(result.find(clause) != clauseList.end()) {
            std::cout << clause << " ";
            result[clause].clause[abs(unitLiteral)] = 0;
        }
    }

    constructLiteralList();

    // Print impled by
    std::cout << "Impled By: ";
    for (int i = 1; i < impledBy.size(); i++) {
        if (impledBy[i])
            std::cout << -1 * impledBy[i] * i << " ";
    }

    char c;
    getchar();

    addTrailInfo(unitLiteral, impledBy);
    printClauseList();

    return result;
}


int CDCL::constructLiteralList() {

    LITERAL tempLiteralList;
    for (CLAUSE::const_iterator it = clauseList.begin(); it != clauseList.end(); it++) {
        for (int i = 1; i < it->second.clause.size(); i++){
            int selectedLiteral = it->second.clause[i]*i;
            if ((it->second.clause[i] != 0) && (tempLiteralList.find(selectedLiteral) == tempLiteralList.end()))
                tempLiteralList[selectedLiteral] = {it->first};

            if((it->second.clause[i] != 0) && !std::count(tempLiteralList[selectedLiteral].begin(), tempLiteralList[selectedLiteral].end(), it->first))
                tempLiteralList[it->second.clause[i]*i].push_back(it->first);
        }
    }
    literalList = tempLiteralList;

    return 1;
}

int CDCL::isConflictPresent() {
    for (CLAUSE::const_iterator it = clauseList.begin(); it != clauseList.end(); it++) {
        bool isConflictClause = true;
        for (int i = 1; i < it->second.clause.size(); i++) {
            isConflictClause = isConflictClause && !it->second.clause[i];
        }
        if (isConflictClause) {
            return it->first;
        }
    }
    return -1;
}

int CDCL::decide() {
    int decisionLiteral = literalList.cbegin()->first;

    trailInfo tempTrailInfo;
    // tempTrailInfo.assignments = previousTrailInfo.assignments;
    // tempTrailInfo.assignments[abs(decisionLiteral)] = abs(decisionLiteral) == decisionLiteral ? true : false;
    currentDecisionLevel += 1;

    std::cout << "** (" << currentDecisionLevel << ")" << " DECIDE --> " << decisionLiteral << std::endl;

    return decisionLiteral;
}

state CDCL::getCurrentState() {
    
    if (isConflictPresent() >= 0)
        return UNSAT;
    if (clauseList.empty())
        return SAT;

    return UNDEF;
}

void CDCL::printCurrentModel() {
    std::cout << std::endl << std::endl << "--- MODEL ---" << std::endl;
    for (int i = 1; i < model.size(); i++) {
        std::cout << i << " --> " << model[i] << std::endl;
    }
    std::cout << "---" << std::endl;
}

CLAUSE CDCL::getClauseList() {
    return clauseList;
}

LITERAL CDCL::getLiteralList() {
    return literalList;
}

int CDCL::setCurrentDecisionLevel(int decisionLevel) {
    currentDecisionLevel = decisionLevel;

    return currentDecisionLevel;
}

trailInfo CDCL::addTrailInfo(int literal, std::vector<int> impledBy) {
    trailInfo temp;
    temp.decisionLevel = currentDecisionLevel;
    temp.impliedBy = impledBy;
    temp.isDecisionLiteral = (!impledBy.empty());
    trail[literal] = temp;
    return temp;
}

int CDCL::addDecisionVariableToMap(int decisionLiteral) {
    decisionLiterals[decisionLiteral] = currentDecisionLevel;
    return currentDecisionLevel;
}

std::vector<int> CDCL::getImpliedByClause(int unitLiteral, int clauseNo, CLAUSE& originalClauseList) {

    std::vector<int> tempClause;
    if (originalClauseList.find(clauseNo) != originalClauseList.end()) {
        tempClause = originalClauseList[clauseNo].clause;
        tempClause[abs(unitLiteral)] = 0;
    }

    return tempClause;
}