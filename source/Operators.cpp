#include "Operators.h"
#include <algorithm>
#include <cassert>

bool Operators::gate_AND(VecGates const& operands, CircuitSAT& obj) {
    return std::all_of(operands.begin(), operands.end(), [&obj](size_t i){return obj.get_gate(i).get_value();});
}

bool Operators::gate_OR(VecGates const& operands, CircuitSAT& obj) {
    return std::any_of(operands.begin(), operands.end(), [&obj](size_t i){return obj.get_gate(i).get_value();});
}

bool Operators::gate_NOT(VecGates const& operands, CircuitSAT& obj) {
    assert(operands.size() == 1 && "Operator NOT expects one gate");
    return obj.get_gate(operands[0]).get_value() != ValueEnum::True;
}

bool Operators::gate_XOR(VecGates const& operands, CircuitSAT& obj) {
    assert(operands.size() == 2 && "Operator XOR/NXOR expects two gates");
    return obj.get_gate(operands[0]).get_value() != obj.get_gate(operands[1]).get_value();
}

bool Operators::gate_NAND(VecGates const& operands, CircuitSAT& obj) {
    return !gate_AND(operands, obj);
}

bool Operators::gate_NOR(VecGates const& operands, CircuitSAT& obj) {
    return !gate_OR(operands, obj);
}

bool Operators::gate_NXOR(VecGates const& operands, CircuitSAT& obj) {
    return !gate_XOR (operands, obj);
}

bool Operators::gate_BUFF(VecGates const& operands, CircuitSAT& obj) {
    assert(operands.size() == 1 && "Operator NOT expects one gate");
    return obj.get_gate(operands[0]).get_value() == ValueEnum::True;
}