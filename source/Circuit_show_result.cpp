#include "Circuit.h"

std::string CircuitSAT::show_result() const {
    if (get_gate(_output_index).get_value() == ValueEnum::True)
        return "SAT";
    return "UNSAT";
}
