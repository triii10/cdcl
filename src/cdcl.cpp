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

CDCL::CDCL(CLAUSE& clauseListCopied, LITERAL& literalListCopied, int variableCount, int clauseCount, std::unordered_map<int, int> previousDecisionLiteralList, std::vector <trailInfo> previousTrailInfo) {

    clauseList = clauseListCopied;
    literalList = literalListCopied;

    model = std::vector <bool> (variableCount+1, false);
    literalCount = variableCount;
    this->clauseCount = clauseCount;
    literalDecisionLevel = previousDecisionLiteralList;
    trail = previousTrailInfo;
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

CLAUSE CDCL::exhaustiveUnitPropagation(CLAUSE& originalClauseList, CLAUSE useThisListInstead) {
    // Unit propagation will find unit clauses, and for each unit clause, it will go through the literalList
    // to find the clauses where the unit literal appears
    // Then, will apply subsumption elimination to remove the clauses, and remove negative occurances of the literal 

    CLAUSE& result = clauseList;
    if (!useThisListInstead.empty())
        result = useThisListInstead;
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

        result = unitPropagation(unitLiteral, originalClauseList);
        if (getCurrentState() == UNSAT)
            return result;
    }

    return result;
}


CLAUSE CDCL::unitPropagation(int unitLiteral, CLAUSE& originalClauseList, bool skipAddToMap) {
    // Unit propagation will find unit clauses, and for each unit clause, it will go through the literalList
    // to find the clauses where the unit literal appears
    // Then, will apply subsumption elimination to remove the clauses, and remove negative occurances of the literal 

    std::vector<int> impledBy;

    if (!unitLiteral)
        return clauseList;

    // First perform subsumption elimination of positive literal to remove clauses
    std::cout << std::endl << "** (" << currentDecisionLevel << ") UP: " << unitLiteral << " --> ";
    model[abs(unitLiteral)] = (abs(unitLiteral) == unitLiteral);

    if (!skipAddToMap)
        addDecisionLevelToMap(unitLiteral);
    for (int clause: literalList[unitLiteral]) {
        // Do clause elimination
        std::cout << clause << " ";

        if (clauseList[clause].unit)
            impledBy = getImpliedByClause(unitLiteral, clause, originalClauseList);
        clauseList.erase(clause);
    }

    std::cout << ", (-) ";
    for (int clause: literalList[-1 * unitLiteral]) {
        // Do negative literal elimination
        if(clauseList.find(clause) != clauseList.end()) {
            std::cout << clause << " ";
            clauseList[clause].clause[abs(unitLiteral)] = 0;
        }
    }

    constructLiteralList();

    // Print impled by
    std::cout << "Implied By: ";
    for (int i = 1; i < impledBy.size(); i++) {
        if (impledBy[i])
            std::cout << -1 * impledBy[i] * i << " ";
    }

    addTrailInfo(unitLiteral, impledBy);
    printClauseList();

    return clauseList;
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
    currentDecisionLevel += 1;
    std::cout << std::endl << "** (" << currentDecisionLevel << ")" << " DECIDE --> " << decisionLiteral << std::endl;

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

int CDCL::getCurrentDecisionLevel() {

    return currentDecisionLevel;
}

trailInfo CDCL::addTrailInfo(int literal, std::vector<int> impledBy) {
    trailInfo temp;
    temp.literal = literal;
    temp.decisionLevel = currentDecisionLevel;
    temp.impliedBy = impledBy;
    temp.isDecisionLiteral = (impledBy.empty() && currentDecisionLevel);
    trail.push_back(temp);
    return temp;
}

int CDCL::addDecisionLevelToMap(int literal) {
    literalDecisionLevel[literal] = currentDecisionLevel;
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


std::vector<int> CDCL::findConflictClause(CLAUSE& originalClauseList) {
    // I have chosen "tri-asserting clause" as the conflict clause.

    // We need to find the impledBy for the UP literal which caused the empty clause
    // And empty clause itself, which contains the negation of the UP literal
    // The conflict clause will be the negation of the impledBy literals of both the UP and negation UP clauses 

    // We can get the impledBy literals of the current UP literal by just looking at the trail stack
    trailInfo lastUpLiteralInfo = trail[trail.size() - 1];
    std::vector <int> upImpledBy = lastUpLiteralInfo.impliedBy;

    // To get the impliedBy of the negation of the UP literal, we need the empty clause.
    int emptyClause = isConflictPresent();
    std::vector <int> negationUpImpledBy = getImpliedByClause(lastUpLiteralInfo.literal, emptyClause, originalClauseList);

    // Try 2 - will find the last unique implication point
    // For this, we need to see all the decision variables in the trail
    std::vector <trailInfo> consolidatedTrailInfo = {};
    std::vector <trailInfo> tempTrailInfo = {};
    for (int i = 0; i < negationUpImpledBy.size(); i++) {
        if (negationUpImpledBy[i]) {
            // std::cout << -negationUpImpledBy[i]*i;
            std::vector <trailInfo> tempTrailInfo = findRoots(-negationUpImpledBy[i]*i);
            consolidatedTrailInfo.insert(consolidatedTrailInfo.end(), tempTrailInfo.begin(), tempTrailInfo.end());
        }
        if (upImpledBy[i]) {
            std::vector <trailInfo> tempTrailInfo = findRoots(-upImpledBy[i]*i);
            consolidatedTrailInfo.insert(consolidatedTrailInfo.end(), tempTrailInfo.begin(), tempTrailInfo.end());
        }
    }

    std::vector <trailInfo> uniqueConsolidatedTrailInfo = {};
    std::unordered_map <int, bool> alreadyAdded;
    
    for (int i = 0; i < consolidatedTrailInfo.size(); i++) {
        int rootLiteral = consolidatedTrailInfo[i].literal;
        if (alreadyAdded.find(rootLiteral) == alreadyAdded.end()) {
            uniqueConsolidatedTrailInfo.push_back(consolidatedTrailInfo[i]);
            alreadyAdded[rootLiteral] = true;
        }
    }

    // std::cout << std::endl << "Decision levels in trail are - ";
    // for (int i = 0; i < trail.size(); i++) {
    //     std::cout << trail[i].literal << (trail[i].isDecisionLiteral ? "(D)" : "") << "->" << trail[i].decisionLevel << " ";
    // }
    // std::cout << std::endl;

    // // Let's print this now
    // std::cout << std::endl << "Roots are - ";
    // for (int i = 0; i < uniqueConsolidatedTrailInfo.size(); i++) {
    //     std::cout << uniqueConsolidatedTrailInfo[i].literal << "->" << uniqueConsolidatedTrailInfo[i].decisionLevel << " ";
    // }
    // std::cout << std::endl;

    // Now construct the conflict clause
    std::vector <int> conflictClause (upImpledBy.size(), 0);
    std::cout << std::endl << "--- CONFLICT CLAUSE : ";
    for (int i = 0; i < uniqueConsolidatedTrailInfo.size(); i++) {
        int rootLiteral = -uniqueConsolidatedTrailInfo[i].literal;
        conflictClause[abs(rootLiteral)] = rootLiteral/abs(rootLiteral);
        std::cout << rootLiteral << " ";
    }
    std::cout << std::endl;

    return conflictClause;
}

int CDCL::getBackjumpLevel(std::vector <int> conflictClause, int conflictDecisionLevel) {

    int maxDecisionLevel = 0;
    int lastDecisionLevel = 0;
    // Get the maximum decision level of conflict clause
    // std::cout << "All Decision Levels: ";
    // for (std::unordered_map<int, int>::const_iterator it = literalDecisionLevel.begin(); it != literalDecisionLevel.end(); it++)
    //     std::cout << it->first << "->" << it->second << ",";
    
    std::cout << "Decision Levels: ";
    for (int i = 1; i < conflictClause.size(); i++) {
        if (conflictClause[i]){
            int currentLiteral = -i*conflictClause[i];
            int decisionLevel = literalDecisionLevel[currentLiteral];
            lastDecisionLevel = decisionLevel;
            std::cout << decisionLevel << " ";
            // if (!decisionLevel)
                if (decisionLevel != conflictDecisionLevel && decisionLevel > maxDecisionLevel)
                    maxDecisionLevel = decisionLevel;
        }
    }
    std::cout << std::endl;
    if (!maxDecisionLevel)
        maxDecisionLevel = lastDecisionLevel;
    if (!maxDecisionLevel)
        maxDecisionLevel = conflictDecisionLevel;
    
    return maxDecisionLevel;
}

std::vector <trailInfo> CDCL::findRoots(int literal) {

    std::vector <trailInfo> consolidatedTrailInfo;
    for (int i = 0; i < trail.size(); i++) {
        if (trail[i].literal == literal) {
            if (trail[i].isDecisionLiteral) 
                consolidatedTrailInfo.push_back(trail[i]);
            else {
                for (int j = 1; j < trail[i].impliedBy.size(); j++) {
                    std::vector <trailInfo> tempTrailInfo = {};
                    if (trail[i].impliedBy[j])
                        tempTrailInfo = findRoots(-trail[i].impliedBy[j]*j);
                    consolidatedTrailInfo.insert(consolidatedTrailInfo.end(), tempTrailInfo.begin(), tempTrailInfo.end());
                }
            }
            break;
        }
    }

    // for (int i = 0; i < consolidatedTrailInfo.size(); i++)
    //     std::cout << consolidatedTrailInfo[i].literal << " ";
    return consolidatedTrailInfo;
}

void CDCL::deleteTrailInfo(int literal) {
    int pos = 0;
    for (int i = 0; i < trail.size(); i++) {
        if (trail[i].literal == literal) {
            pos = i;
            break;
        }
    }
    trail.erase(trail.begin() + pos);
}