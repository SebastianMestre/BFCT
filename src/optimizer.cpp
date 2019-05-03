#include "optimizer.hpp"

std::vector<Op> collapse_consecutive(std::vector<Op> const& code)
{
    std::vector<Op> result;

    int count = 0;
    Opcode opcode = Opcode::Count; // dummy value
    for (Op op : code)
    {
        if (op.opcode != opcode and count)
        {
            result.push_back(multi_op(opcode, count));
        }

        if (not has_multi(op.opcode))
        {
            result.push_back(op);
            opcode = Opcode::Count;
            count = 0;
        }
        else
        {
            if (op.opcode == opcode)
            {
                count++;
            }
            else
            {
                opcode = op.opcode;
                count = 1;
            }
        }
    }

    return result;
}

std::vector<Op> constant_folding(std::vector<Op> const& code)
{
    std::vector<Op> result;

    int i = 0;
    for (; i < code.size() - 2; ++i)
    {
        if (code[i].opcode == Opcode::Lop)
        {
            if (code[i + 1].opcode == Opcode::Inc || code[i + 1].opcode == Opcode::Dec
                || code[i + 1] == Op{ Opcode::Addi, 1 } || code[i + 1] == Op{ Opcode::Subi, 1 })
            {
                if (code[i + 2].opcode == Opcode::Lcl)
                {
                    result.push_back(Op{Opcode::Seti, 0});
                    i += 2;
                    continue;
                }
            }
        }

        // this only runs if the check above fails somewhere
        result.push_back(code[i]);
    }
    
    for (; i < code.size(); ++i) {
        result.push_back(code[i]);
    }

    return result;
}

std::vector<Op> optimize(std::vector<Op> const& code)
{
    std::vector<Op> result = code;
    
    result = collapse_consecutive(result);
    result = constant_folding(result);

    return result;
}


