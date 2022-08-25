#include "Circuit.h"
#include <string>
#include <fstream>
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc == 3) {

        std::ofstream out1(argv[2], std::ios::app);
        out1 << argv[1];
        CircuitSAT circuit;

        circuit.parse(argv[1]);
        out1 << " -- " + std::to_string(circuit.get_gates().size()) + "; ";

        circuit.simplify();
        out1 << std::to_string(circuit.get_gates().size()) + " => ";
        out1.close();

        std::ofstream out2(argv[2], std::ios::app);
        circuit.solve();
        out2 << circuit.show_result() + "\n";
        out2.close();

        return 0;
    }

    return 1;
}
