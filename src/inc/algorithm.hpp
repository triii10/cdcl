#include "cdcl.hpp"
#include "io.hpp"

class Algorithm {

    public:
        state runAlgorithm(CLAUSE, LITERAL, int, int);
        void printResult(state&);
};