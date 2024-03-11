#include "cdcl.hpp"
#include "io.hpp"

struct resultStruct {
    state currentState;
    std::vector<int> conflictClause;
    int backjumpLevel;
};

class Algorithm {
    private:
        CLAUSE originalClauseList = {};
        LITERAL originalLiteralList = {};
        std::vector <int> getImpliedByClause(int, int);
    public:
        resultStruct runAlgorithm(CLAUSE, LITERAL, int, int, int = 0, std::unordered_map<int, int> = {}, std::vector<trailInfo> = {});
        void printResult(state&);
};