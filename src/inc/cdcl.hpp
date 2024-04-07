#include "structure.hpp"
#include <vector>
#include <unordered_map>

class CDCL {
    private:

        // clauseList is the list of clauses, where each entry in the vector corresponds to a clause,
        // such that, 
        //      if the value at index "i" is 0, it means the literal "i" does not exist in the clause
        //      if the value at index "i" is 1, it means the literal "i" is positive in the clause
        //      if the value at index "i" is -1, it means the literal "i" is negated in the clause
        // Note: If an entry contains all 0, it means the clauses are unsatisfiable
        std::unordered_map < int, clauseInfo > clauseList;

        // literalList contains a list of all clauses, where a particular literal appears.
        // For index "i" of literalList, if the value is {1, 3} meaning, literal "i" appears in clause 1 and 3
        // referencing to clauseList index.
        // Note: 0 is the first clause.
        std::unordered_map < int, std::vector <int> > literalList;

        // To store the interpretation
        std::vector <bool> model;

        // To store the trail for backjumping
        std::vector <trailInfo> trail;

        // Store current decision level
        int currentDecisionLevel = 0;

        // Clause and Literal count
        int clauseCount;
        int literalCount;

        std::unordered_map <int, int> literalDecisionLevel;

        trailInfo addTrailInfo(int, std::vector<int>);

    public:
        CDCL(CLAUSE&, LITERAL&, int, int, std::unordered_map<int, int> = {}, std::vector <trailInfo> = {});
        void printClauseList(const std::unordered_map< int, clauseInfo >&);
        void printClauseList();
        void deleteTrailInfo(int);

        CLAUSE getClauseList();
        LITERAL getLiteralList();

        int constructLiteralList();
        void printLiteralList();

        int findUnitClauses();
        int isConflictPresent();
        state getCurrentState();
        void printCurrentModel();
        std::vector<int> getCurrentModel();
        int setCurrentDecisionLevel(int);
        int getCurrentDecisionLevel();
        int getBackjumpLevel(std::vector <int>, int);
        std::unordered_map<int, int> getLiteralDecisionLevelList() {
            return literalDecisionLevel;
        };
        std::vector <trailInfo> getTrailInfo() {
            return trail;
        };
    
        std::unordered_map<int, clauseInfo> exhaustiveUnitPropagation(CLAUSE&, CLAUSE = {});
        std::unordered_map<int, clauseInfo> unitPropagation(int unitLiteral, CLAUSE&, bool = false);
        std::vector<int> getImpliedByClause(int, int, CLAUSE&);
        int addDecisionLevelToMap(int);
        int decide();
        std::vector<int> findConflictClause(CLAUSE&);
        std::vector<trailInfo> findRoots(int);
        // int runAlgorithm();
        // std::vector <int> getModel();
};