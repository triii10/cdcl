#include "cdcl.hpp"
#include <iostream>

#define CLAUSE std::unordered_map<int, clauseInfo>
#define LITERAL std::unordered_map < int, std::vector <int> >

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

CDCL::CDCL(std::vector< std::vector<int> > inputList, int variableCount, int clauseCount) {
    
    model = std::vector <bool> (variableCount+1, false);

    for (int clause = 0; clause < inputList.size(); clause++) {

        std::vector <int> tempClause(variableCount+1, 0);     // Initialize a temp clause with all 0

        bool invalid = false;
        for (int literal = 0; literal < inputList[clause].size(); literal++) {
            int literalNo = inputList[clause][literal];
            int absLiteralNo = abs(literalNo);
            // If the literal already exists, and had a sign different than the one being added currently, 
            // then this clause is already true, so we may as well discard the clause.
            if (tempClause[absLiteralNo] != 0 && (std::signbit(tempClause[absLiteralNo]) ^ std::signbit(literalNo))) {
                invalid = true;
                break;
            }
            tempClause[absLiteralNo] = (literalNo/absLiteralNo);

            // Now add an entry for this clause in the literalList
            if (literalList.find(literalNo) == literalList.end())
                literalList[literalNo] = {clause};

            if(!std::count(literalList[literalNo].begin(), literalList[literalNo].end(), clause))
                literalList[literalNo].push_back(clause);
        }

        if (invalid)
            continue;

        clauseInfo tempClauseInfo;
        tempClauseInfo.unit=false;
        tempClauseInfo.unitLiteral=0;
        tempClauseInfo.clause=tempClause;

        clauseList[clause] = tempClauseInfo;
    }
}

CDCL::CDCL(CLAUSE& clauseListCopied, LITERAL& literalListCopied) {
    clauseList = clauseListCopied;
    literalList = literalListCopied;
}

int CDCL::findUnitClauses() {
    // Idea is to parse through all clauses and see which has only one literal
    // Then update the index 0 in clauseList for that clause to the literal which is unit.

    bool anyUnitClauseExists = false;

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

                anyUnitClauseExists = true;
                hasUnitClause = true;
                unitClauseNo = it->second.clause[literalInClause]*literalInClause;
            }
        }
        if (hasUnitClause)
            clauseList[it->first].unitLiteral = unitClauseNo;
            clauseList[it->first].unit = hasUnitClause;
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
    std::cout << std::endl << "------" << std::endl;
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

CLAUSE CDCL::unitPropagation() {
    // Unit propagation will find unit clauses, and for each unit clause, it will go through the literalList
    // to find the clauses where the unit literal appears
    // Then, will apply subsumption elimination to remove the clauses, and remove negative occurances of the literal 

    CLAUSE& result = clauseList;
    int unitLiteral = 0;
    for (CLAUSE::const_iterator it = clauseList.begin(); it != clauseList.end(); it++) {
        clauseInfo tempClauseInfo = it->second;
        if (tempClauseInfo.unit) {
            unitLiteral = tempClauseInfo.unitLiteral;
            break;
        }
    }

    // First perform subsumption elimination of positive literal to remove clauses
    std::cout << "UP: " << unitLiteral << std::endl;
    model[abs(unitLiteral)] = (abs(unitLiteral) == unitLiteral);

    for (int clause: literalList[unitLiteral]) {
        // Do clause elimination
        result.erase(clause);
    }

    for (int clause: literalList[-1 * unitLiteral]) {
        // Do negative literal elimination
        if(result.find(clause) != clauseList.end()) {
            result[clause].clause[abs(unitLiteral)] = 0;
        }
    }
    if (findUnitClauses()) {
        printClauseList();
        return unitPropagation();
    }

    constructLiteralList();

    std::cout << model;
    return result;
}

int CDCL::constructLiteralList() {

    LITERAL tempLiteralList;
    for (CLAUSE::const_iterator it = clauseList.begin(); it != clauseList.end(); it++) {
        for (int i = 1; i < it->second.clause.size(); i++){
            if (it->second.clause[i] && (tempLiteralList.find(1) == tempLiteralList.end()))
                tempLiteralList[it->second.clause[i]*i] = {it->first};

            if(it->second.clause[i] && !std::count(tempLiteralList[it->second.clause[i]*i].begin(), tempLiteralList[it->second.clause[i]*i].end(), it->first))
                tempLiteralList[it->second.clause[i]*i].push_back(it->first);
        }
    }
    literalList = tempLiteralList;

    return 1;
}

int CDCL::isConflictPresent(state& currentState) {
    for (CLAUSE::const_iterator it = clauseList.begin(); it != clauseList.end(); it++) {
        bool isConflictClause = true;
        for (int i = 1; i < it->second.clause.size(); i++) {
            isConflictClause = isConflictClause && !it->second.clause[i];
        }
        if (isConflictClause) {
            currentState = UNSAT;
            return it->first;
        }
    }
    currentState = UNDEF;
    return -1;
}

int CDCL::decide(trailInfo previousTrailInfo) {
    int decisionLiteral = literalList.cbegin()->first;

    trailInfo tempTrailInfo;
    tempTrailInfo.decisionLevel = previousTrailInfo.decisionLevel + 1;
    tempTrailInfo.assignments = previousTrailInfo.assignments;
    tempTrailInfo.assignments[abs(decisionLiteral)] = abs(decisionLiteral) == decisionLiteral ? true : false;

    trail.push_back(tempTrailInfo);
}