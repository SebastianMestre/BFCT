#include "optimizer.hpp"

#include <algorithm>
#include <utility>
#include <map>

std::vector<Op> collapse_consecutive(std::vector<Op> const& code)
{
    std::vector<Op> result;

    int count = 0;
    Opcode opcode = Opcode::Count; // dummy value
    for (Op op : code)
    {
        if (op.opcode != opcode and count)
        {
            result.push_back(Op{ opcode, int16_t(count) });
        }

        if (not can_merge(op.opcode))
        {
            result.push_back(op);
            opcode = Opcode::Count;
            count = 0;
        }
        else
        {
            if (op.opcode == opcode)
            {
                count += op.arg1;
            }
            else
            {
                opcode = op.opcode;
                count = op.arg1;
            }
        }
    }
    if (count)
    {
        result.push_back(Op{ opcode, int16_t(count) });
    }

    return result;
}

void insert_simplified_ops(std::vector<Op>& target, std::vector<Op> const& code, int low, int high)
{
	assert(code[low].opcode == Opcode::Lop);
	assert(code[high].opcode == Opcode::Lcl);
	int shift = 0;

	for (int i = low + 1; i < high; ++i)
	{
		if (code[i].opcode == Opcode::Jmp)
		{
			shift += code[i].arg1;
		}
	}

	// No easy optimization possible
	if (shift != 0)
		goto nooptimize;

	{
		int pos = 0;
		std::map<int, int> deltas;
		for (int i = low + 1; i < high; ++i)
		{
			if (code[i].opcode == Opcode::Jmp)
				pos += code[i].arg1;
			else if (code[i].opcode == Opcode::Addi)
				deltas[pos] += code[i].arg1;
			else
				goto nooptimize; // No easy optimization possible
		}

		// No easy optimization possible
		if (deltas[0] != -1)
			goto nooptimize;

		// This vector is the order in which we are going to do operations.
		// first = position
		// second = difference
		{
			std::vector<std::pair<int, int>> pairs;
			// Start by jumping to offset zero, with no difference
			pairs.push_back({ 0, 0 });
			// Then to every other offset in increasing order.
			for (auto kv : deltas)
				if (kv.first != 0)
					pairs.push_back(kv);

			// Now we jump from every position to the next.
			// M-ADDing as we go
			std::adjacent_find(
			    pairs.begin(), pairs.end(), [&](auto const& lhs, auto const& rhs) {
				    // Distance between positions
				    int diff = rhs.first - lhs.first;

				    target.push_back(Op{ Opcode::Jmp, int16_t(diff) });
				    target.push_back(
				        Op{ Opcode::Madd, int16_t(rhs.second), int16_t(-rhs.first) });

					// I am using adjacent_find as pair-wise foreach, hence the
					// false predicate
				    return false; 
			    });

			// Jump back to offset 0:
			target.push_back(Op{ Opcode::Jmp, int16_t(-pairs.back().first) });

			// Set it to zero
			target.push_back(Op{ Opcode::Seti, 0 });
		}
	}

	return;
nooptimize:

	for (int i = low; i <= high; ++i)
		target.push_back(code[i]);

	return;
}

std::vector<Op> constant_folding(std::vector<Op> const& code)
{
    std::vector<Op> result;

    int last = 0;
    bool last_open = false;

    for(int i = 0; i < code.size(); ++i) {
        if(code[i].opcode == Opcode::Lop){
            for(int j = last; j < i; ++j)
                result.push_back(code[j]);
            last = i;
            last_open = true;
        }

        if(code[i].opcode == Opcode::Lcl and last_open){
            last_open = false;

            insert_simplified_ops(result, code, last, i);

            last = i+1;
        }
    }

    for(; last < code.size(); ++last) {
        result.push_back(code[last]);
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


