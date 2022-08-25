#include "Circuit.h"
#include <map>

void CircuitSAT::simplify() {
    /** remove gates that do not affect the output or are a copy of another **/
    _backpropagation_to_use(_output_index);
    _replace_copy_gates();
    _remove_unused_gates();
    _rename_gates();
}

void CircuitSAT::_backpropagation_to_use(GateIdx idx) {
    /** recursive backpropagation of gate influence on the result **/

    if (get_gate(idx).get_used_by_output_value() != ValueEnum::True) { // gate not viewed yet?
        set_gate_using(idx, ValueEnum::True);

        for (GateIdx operand : get_gate(idx).get_operand_indexes()) {
            _backpropagation_to_use(operand);
        }
        
    }
}

std::string encode_gate_name(size_t pos_gate, CircuitSAT& obj) {
    std::string name = std::to_string(static_cast<int>(obj.get_gate(pos_gate).get_operator_type()));
    for (GateIdx operand : obj.get_gate(pos_gate).get_operand_indexes()) {
        name += "_" + std::to_string(operand);
    }

    return name;
}

void change_gate_index(size_t old_index, size_t new_index, CircuitSAT& obj) {
    /**
     * changes the operands of those gates that used the old gate
     **/
    for (size_t child : obj.get_gate(old_index).get_children_indexes()) {
        VecGates operands = obj.get_gate(child).get_operand_indexes();
        for (size_t pos_operand = 0; pos_operand != operands.size(); ++pos_operand) {
            if (operands[pos_operand] == obj.get_gate(old_index).get_gate_index()) {
                obj.change_gate_operand(child, pos_operand, new_index);
            }
        }
    }
}

void CircuitSAT::_replace_copy_gates() {
    /**
     * finds gates with the same operands and operator,
     * takes the first one as a basis,
     * changes the operands of all gates where the second gate was used
     * and marks the second gate as unnecessary
     **/
    std::map<std::string, GateIdx> map_gate_name_to_idx;
    std::string encoded_name;

    for (size_t gate_index = 0; gate_index != get_gates().size(); ++gate_index) {
        if (get_gate(gate_index).get_operator_type() != OperatorsEnum::INPUT
                && get_gate(gate_index).get_used_by_output_value() == ValueEnum::True) {

            encoded_name = encode_gate_name(get_gate(gate_index).get_gate_index(), *this);

            if (map_gate_name_to_idx.find(encoded_name) == map_gate_name_to_idx.end()) {
                map_gate_name_to_idx[encoded_name] = get_gate(gate_index).get_gate_index();

            } else {
                change_gate_index(gate_index, map_gate_name_to_idx.at(encoded_name), *this);
                set_gate_using(gate_index, ValueEnum::False);
            }
        }
    }
}

void CircuitSAT::_remove_unused_gates() {
    std::vector<Gate> new_gates;
    for (size_t idx = 0; idx != get_gates().size(); ++idx) {
        if (get_gate(idx).get_used_by_output_value() == ValueEnum::True) {
            new_gates.push_back(get_gate(idx));
        }
    }
    _gates = new_gates;
}

void CircuitSAT::_rename_gates() {
    /** rename gates that remain after circuit transformations **/

    // fill in the dictionary and change the indexes in the struct Gate
    std::map<GateIdx, GateIdx> map_old_to_new_idx;
    GateIdx new_index = 0;
    for (size_t pos_index = 0; pos_index != get_gates().size(); ++pos_index) {
        map_old_to_new_idx[get_gate(pos_index).get_gate_index()] = new_index;
        set_gate_index(pos_index, new_index);
        ++new_index;
    }

    // change operand indices in the class CircuitSAT and assemble a new vector of input gates
    clear_input_gate_indexes();
    for (size_t pos_gate = 0; pos_gate != get_gates().size(); ++pos_gate) {
        for (size_t num_operand = 0; num_operand != get_gate(pos_gate).get_operand_indexes().size(); ++num_operand) {
            change_gate_operand(
                    pos_gate,
                    num_operand,
                    map_old_to_new_idx[get_gate(pos_gate).get_operand_index(num_operand)]
            );
        }
        if (get_gate(pos_gate).get_operator_type() == OperatorsEnum::INPUT) {
            append_input_gate(pos_gate);
        }
    }

    // change output
    set_idx_output(map_old_to_new_idx[_output_index]);

}

