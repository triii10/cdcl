#include "inc/io.hpp"

void Dimacs::Input() {

    std::cout << "Input your dimacs file below:\n";
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

        clauseList.push_back(tempClause);
    }
}

void Dimacs::Output() {
    std::cout << "\nThe clauses are: \n";
    for (int i = 0; i < countClauses; i++) {
        for (int j = 0; j < clauseList[i].size(); j++) {
            std::cout << clauseList[i][j] << " ";
        }
        std::cout << "\n";
    }
}

int Dimacs::getClauseCount() {
    return countClauses;
}

int Dimacs::getVarCount() {
    return countVar;
}

std::vector <std::vector <int> > Dimacs::getClauseList() {
    return clauseList;
}