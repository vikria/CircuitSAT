#include "Circuit.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <cassert>

constexpr size_t len_input = 6;  // "INPUT("
constexpr size_t len_output = 7; // "OUTPUT("

using OperMap = std::map<std::string, OperatorsEnum>;
inline OperatorsEnum str_to_enum_operator(const std::string& str) {
    static OperMap str_to_enum { /** encode string to enum **/
            {"INPUT", OperatorsEnum::INPUT},
            {"AND"  , OperatorsEnum::AND},
            {"OR"   , OperatorsEnum::OR},
            {"NOT"  , OperatorsEnum::NOT},
            {"XOR"  , OperatorsEnum::XOR},
            {"NAND" , OperatorsEnum::NAND},
            {"NOR"  , OperatorsEnum::NOR},
            {"NXOR" , OperatorsEnum::NXOR},
            {"BUFF" , OperatorsEnum::BUFF}
        };

    return str_to_enum.at(str);
}

void str_trim(std::string& line) { /** easy file preprocessing: remove whitespace */
    line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
}

std::string substr_between_brackets(std::string str) { /** name of gate that is between the brackets */
    int start_index = 0;
    for (; str[start_index] != '('; ++start_index) {}

    int end_index = start_index + 1;
    for (; str[end_index] != ')'; ++end_index) {}

    return str.substr(start_index + 1, end_index - start_index - 1);
}

void CircuitSAT::parse(std::string const& path) { /** parsing file **/
    std::ifstream bench_file(path);
    assert(bench_file.is_open() && "Failed to open Bench file");

    std::string line;
    std::string output_name = "UNTITLED";
    std::map<std::string, GateIdx> map_gates; // [name_gates -> number_gates]
    GateIdx num_gates = 0;

    while (getline(bench_file, line))
    {
        str_trim(line);
        if (line.length() == 0 || line[0] == '#') { // comment in the file
            continue;
        }

        if (line.size() > len_input && line.substr(0, len_input) == "INPUT(") {

            std::string input_name = substr_between_brackets(line);

            if (map_gates.find(input_name) == map_gates.end()) { // create new gate if we haven't seen it yet
                map_gates[input_name] = num_gates;
                append_gates(Gate(input_name, num_gates));
                ++num_gates;
            }

            GateIdx input_index = map_gates[input_name];
            append_input_gate(input_index); // store the encoded name
            set_gate_operator(input_index, str_to_enum_operator("INPUT"));
            
        } else if (line.size() > len_output && line.substr(0, len_output) == "OUTPUT(") {

            assert(output_name == "UNTITLED" && "You have more than one output");
            output_name = substr_between_brackets(line); // store the output name. We initialize at the end of
            // the function, after reading its operator and operands

        } else if (line.find('=') != std::string::npos) {

            // Read line by character, filling in the class structure along the way
            size_t start_index = 0;
            size_t end_index = 0;

            // gate name
            for(; line[end_index] != '='; ++end_index) {}
            std::string gate_name = line.substr(start_index, end_index);
            if (map_gates.find(gate_name) == map_gates.end()) { // create new gate
                map_gates[gate_name] = num_gates;
                append_gates(Gate(gate_name, num_gates));
                ++num_gates;
            }
            start_index = end_index + 1;

            // operator
            for(; line[end_index] != '('; ++end_index) {}
            std::string operator_name = line.substr(start_index, end_index - start_index);

            set_gate_operator(map_gates[gate_name], str_to_enum_operator(operator_name));
            start_index = end_index + 1;

            // operands
            for(; end_index != line.size() && line[end_index] != '#'; ++end_index){
                if (line[end_index] == ',' || line[end_index] == ')'){

                    std::string operand = line.substr(start_index, end_index - start_index);
                    if (map_gates.find(operand) == map_gates.end()){ // create new gate
                        map_gates[operand] = num_gates;
                        append_gates(Gate(operand, num_gates));
                        ++num_gates;
                    }
                    append_gate_operand_index(map_gates[gate_name], map_gates[operand]);
                    append_gate_child_index(map_gates[operand], map_gates[gate_name]);
                    start_index = end_index + 1;
                }
            }

        } else {
            std::cerr << "I cant read it: " + line << std::endl;
            exit(1);
        }
    }

    assert(output_name != "UNTITLED" && "You haven't got output");
    set_idx_output(map_gates[output_name]); // store the encoded output name
}
