#include "Circuit.h"
#include "Operators.h"
#include <map>

using operator_ = bool(*)(VecGates const&, CircuitSAT&);
using func_map = std::map<OperatorsEnum, operator_>;

inline operator_ getOperatorMap(OperatorsEnum _operator) {
    static func_map oper_to_func { /** connection of operators and their functions **/
            {OperatorsEnum::AND, &Operators::gate_AND},
            {OperatorsEnum::OR, &Operators::gate_OR},
            {OperatorsEnum::NOT, Operators::gate_NOT},
            {OperatorsEnum::XOR, &Operators::gate_XOR},
            {OperatorsEnum::NAND, &Operators::gate_NAND},
            {OperatorsEnum::NOR, Operators::gate_NOR},
            {OperatorsEnum::NXOR, &Operators::gate_NXOR},
            {OperatorsEnum::BUFF, &Operators::gate_BUFF}
    };

    return oper_to_func.at(_operator);
}

bool CircuitSAT::solve() {
    return _solve(0, ValueEnum::True) || _solve(0, ValueEnum::False);
}

void reset_value(CircuitSAT& obj) { // reset the old values
    for (size_t idx_gate = 0; idx_gate != obj.get_gates().size(); ++idx_gate) {
        if (obj.get_gate(idx_gate).get_operator_type() == OperatorsEnum::INPUT) {
            obj.set_gate_status_calculate(idx_gate, ValueEnum::True);
        } else {
            obj.set_gate_status_calculate(idx_gate, ValueEnum::NotDetermined);
        }
    }
}

void calculate_gate(size_t pos_gate, CircuitSAT& obj) {
    /** recursive solution of the circuit for given values of input gates **/

    for (size_t idx_operand : obj.get_gate(pos_gate).get_operand_indexes()) { // check gate operands
        // 0 - needs to be solved, 1 - has already been solved at the previous stages
        if (obj.get_gate(idx_operand).get_status_calculate() != ValueEnum::True) {
            calculate_gate(idx_operand, obj);
        }
    }

    // calculate the gate itself
    obj.set_gate_status_calculate(pos_gate, ValueEnum::True);
    operator_ op = getOperatorMap(obj.get_gate(pos_gate).get_operator_type());
    bool res = op(obj.get_gate(pos_gate).get_operand_indexes(), obj);

    if (res) {
        obj.set_gate_value(pos_gate, ValueEnum::True);
    } else {
        obj.set_gate_value(pos_gate, ValueEnum::False);
    }

}

bool CircuitSAT::_solve(size_t pos_input_gate, ValueEnum value) {
    /** recursive enumeration of possible input gate values + get circuit result **/
    set_gate_value(get_input_gate_index(pos_input_gate), value);

    if (pos_input_gate == get_input_gate_indexes().size() - 1) {

        reset_value(*this);
        calculate_gate(_output_index, *this);
        return get_gate(_output_index).get_value() == ValueEnum::True;
    }

    return _solve(pos_input_gate + 1, ValueEnum::True) || _solve(pos_input_gate + 1, ValueEnum::False);
}
