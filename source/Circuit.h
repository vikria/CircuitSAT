#pragma once

#include <string>
#include <utility>
#include <vector>
#include <map>

enum class OperatorsEnum { /** gate operators */
    UNKNOWN,
    INPUT,
    AND,
    OR,
    NOT,
    XOR,
    NAND,
    NOR,
    NXOR,
    BUFF
};

enum class ValueEnum { /** value gates */
    NotDetermined = -1,
    False,
    True
};

using GateIdx = size_t;
using VecGates = std::vector<GateIdx>;

struct Gate {
    /**
     * Struct to represent gate
     * @private_fields:
     *      _gate_name          -- gate name from file
     *      _operand_indexes    -- gate operands
     *      _children_indexes   -- indexes of gates that use the current gate as an operand
     *      _gate_index         -- gate index in vector from CircuitSAT class
     *      _operator_type      -- operator type (OperatorsEnum)
     *      _value              -- the resulting value of the gate, after initializing the input gates and
     *                             calculating the values of its operands
     *      _used_by_output     -- effect of the gate on the result of the output
     **/

    public:
        Gate(
            std::string gate_name,
            GateIdx gate_index,
            VecGates operand_indexes = {},
            VecGates children_indexes = {},
            OperatorsEnum operator_type = OperatorsEnum::UNKNOWN,
            ValueEnum value = ValueEnum::NotDetermined,
            ValueEnum used_by_output = ValueEnum::NotDetermined,
            ValueEnum need_to_be_calculate = ValueEnum::NotDetermined

        )
          : _gate_name(std::move(gate_name))
          , _operand_indexes(std::move(operand_indexes))
          , _children_indexes(std::move(children_indexes))
          , _gate_index(gate_index)
          , _operator_type(operator_type)
          , _value(value)
          , _used_by_output(used_by_output)
          , _need_to_be_calculate(need_to_be_calculate) {};

        ~Gate() = default;

        friend class CircuitSAT;

        [[maybe_unused, nodiscard]] std::string const& get_name()   const {return _gate_name;}
        [[nodiscard]] VecGates const& get_operand_indexes()         const {return _operand_indexes;}
        [[nodiscard]] GateIdx get_operand_index(size_t pos)         const {return _operand_indexes.at(pos);}
        [[nodiscard]] VecGates const& get_children_indexes()        const {return _children_indexes;}
        [[nodiscard]] GateIdx get_gate_index()                      const {return _gate_index;}
        [[nodiscard]] OperatorsEnum get_operator_type()             const {return _operator_type;}
        [[nodiscard]] ValueEnum get_value()                         const {return _value;}
        [[nodiscard]] ValueEnum get_status_calculate()              const {return _need_to_be_calculate;}
        [[nodiscard]] ValueEnum get_used_by_output_value()          const {return _used_by_output;}

    private:
        void append_operand_index(GateIdx operand)                        {_operand_indexes.push_back(operand);}
        void change_operand(size_t pos, GateIdx new_operand)              {_operand_indexes.at(pos) = new_operand;}
        void append_child_index(GateIdx child)                            {_children_indexes.push_back(child);}
        void set_operator(OperatorsEnum op)                               {_operator_type = op;}
        void set_value(ValueEnum value)                                   {_value = value;}
        void set_using(ValueEnum value)                                   {_used_by_output = value;}
        void set_status_calculate(ValueEnum value)                        {_need_to_be_calculate = value;}
        void set_index(GateIdx gate_index)                                {_gate_index = gate_index;}

        std::string _gate_name;
        VecGates _operand_indexes;
        VecGates _children_indexes;
        GateIdx _gate_index;
        OperatorsEnum _operator_type;
        ValueEnum _value;
        ValueEnum _used_by_output;
        ValueEnum _need_to_be_calculate;
};

class CircuitSAT {
    /**
     * Struct to represent circuit
     * @fields:
     *     _input_gate_indexes  -- encoded name gate vector
     *     _gates               -- vector of Gate
     *     _output_index        -- encoded name output gate
     *
     * @methods:
     *     parse                -- parsing file
     *     simplify             -- remove gates that do not affect the output or are a copy of another
     *     solve                -- recursive full enumeration of possible values of input gates
     *     show_result          -- result after solve circuit (SAT/UNSAT)
     **/

    public:
        // get fields in class CircuitSAT
        [[nodiscard]] VecGates const& get_input_gate_indexes() const {return _input_gate_indexes;}
        [[nodiscard]] size_t get_input_gate_index(size_t pos)  const {return _input_gate_indexes.at(pos);}
        [[nodiscard]] std::vector<Gate> const& get_gates()     const {return _gates;}
        [[nodiscard]] Gate const& get_gate(size_t pos)         const {return _gates.at(pos);}

        // set fields in class CircuitSAT
        void append_input_gate(GateIdx idx)                          {_input_gate_indexes.push_back(idx);}
        void append_gates(Gate&& gate)                               {_gates.push_back(std::move(gate));}
        void set_idx_output(GateIdx idx)                             {_output_index = idx;}

        // delete fields in class CircuitSAT
        void clear_input_gate_indexes()                              {_input_gate_indexes={};}

        // set fields in struct Gate
        void append_gate_operand_index(size_t pos, GateIdx operand)  {_gates.at(pos).append_operand_index(operand);}
        void change_gate_operand(size_t pos, size_t pos_op, size_t new_op)
                                                                     {_gates.at(pos).change_operand(pos_op, new_op);}
        void append_gate_child_index(size_t pos, GateIdx child)      {_gates.at(pos).append_child_index(child);}
        void set_gate_index(size_t pos, GateIdx new_idx)             {_gates.at(pos).set_index(new_idx);}
        void set_gate_operator(size_t pos, OperatorsEnum op)         {_gates.at(pos).set_operator(op);}
        void set_gate_value(size_t pos, ValueEnum value)             {_gates.at(pos).set_value(value);}
        void set_gate_using(size_t pos, ValueEnum value)             {_gates.at(pos).set_using(value);}
        void set_gate_status_calculate(size_t pos, ValueEnum value)  {_gates.at(pos).set_status_calculate(value);}

        // functions to solve the circuit
        void parse(std::string const& path);
        void simplify();
        bool solve();
        [[nodiscard]] std::string show_result() const;

        friend struct Operators;

    private:
        bool _solve(size_t pos_input_gate, ValueEnum value);
        void _backpropagation_to_use(GateIdx idx);
        void _replace_copy_gates();
        void _remove_unused_gates();
        void _rename_gates();

        VecGates _input_gate_indexes;
        std::vector<Gate> _gates;
        GateIdx _output_index = 0;

};

