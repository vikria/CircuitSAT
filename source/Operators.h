#pragma once

#include "Circuit.h"

struct Operators {
    /** Struct with the implementation of operator functions for getting the value of the gate **/
    public:
        static bool gate_AND(VecGates const& operands, CircuitSAT& obj);
        static bool gate_OR(VecGates const& operands, CircuitSAT& obj);
        static bool gate_NOT(VecGates const& operands, CircuitSAT& obj);
        static bool gate_XOR(VecGates const& operands, CircuitSAT& obj);
        static bool gate_NAND(VecGates const& operands, CircuitSAT& obj);
        static bool gate_NOR(VecGates const& operands, CircuitSAT& obj);
        static bool gate_NXOR(VecGates const& operands, CircuitSAT& obj);
        static bool gate_BUFF(VecGates const& operands, CircuitSAT& obj);

    private:
        Operators() = default;
};
