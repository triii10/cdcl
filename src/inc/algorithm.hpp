#include "cdcl.hpp"
#include "io.hpp"

class Algorithm {
    private:
        CLAUSE originalClauseList = {};
        LITERAL originalLiteralList = {};
        std::vector <int> getImpliedByClause(int, int);
    public:
        state runAlgorithm(CLAUSE, LITERAL, int, int, TRAIL = {});
        void printResult(state&);
};