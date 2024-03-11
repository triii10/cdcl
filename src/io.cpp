#include "inc/io.hpp"

void Dimacs::Input(CLAUSE& clauseList, LITERAL& literalList) {

    // std::cout << "Input your dimacs file below:\n";
    std::string line;

    while(1) {
        std::getline(std::cin, line);

        if (line.empty())
            continue;
        
        if (line[0] == 'c')
            continue;
        
        if (line[0] == 'p') {
            std::istringstream strStream(line.substr(5, line.length()));    // Cut "p cnf" part and read variables and clauses
            
            // Read the integers out of the line. First integer is the count of variables
            // Second integer is the count of clauses
            strStream >> countVar;
            strStream >> countClauses;

            std::cout << "Number of clauses: "<< countClauses << "\nNumber of variables: "<< countVar;
            break;
        }
    }

    // Once the count of variables and clauses are read, proceed to read the clauses
    for (int i = 0; i < countClauses; i++) {
        std::getline(std::cin, line);

        while (line.empty())
            std::getline(std::cin, line);

        std::istringstream strStream(line);
        std::vector<int> tempClause;

        int predicate;
        while(strStream >> predicate) {
            if (!predicate)
                break;

            tempClause.push_back(predicate);
        }

        rawInputList.push_back(tempClause);
    }

    for (int clause = 0; clause < rawInputList.size(); clause++) {

        std::vector <int> tempClause(countVar + 1, 0);     // Initialize a temp clause with all 0

        bool invalid = false;
        for (int literal = 0; literal < rawInputList[clause].size(); literal++) {
            int literalNo = rawInputList[clause][literal];
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


// void Dimacs::Output() {
//     std::cout << "\nThe clauses are: \n";
//     for (int i = 0; i < countClauses; i++) {
//         for (int j = 0; j < clauseList[i].size(); j++) {
//             std::cout << clauseList[i][j] << " ";
//         }
//         std::cout << "\n";
//     }
// }


int Dimacs::getClauseCount() {
    return countClauses;
}

int Dimacs::getVarCount() {
    return countVar;
}