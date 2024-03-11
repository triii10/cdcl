#include "main.hpp"

int main(int argc, char const *argv[])
{

    // Take inputs from the stdin redirection
    Dimacs inp;
    CLAUSE c;
    LITERAL l;
    inp.Input(c, l);

    Algorithm A;
    resultStruct S = A.runAlgorithm(c, l, inp.getClauseCount(), inp.getVarCount());
    A.printResult(S.currentState);

    return 0;
}
