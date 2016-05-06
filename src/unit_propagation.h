#ifndef _UNIT_PROP
#define _UNIT_PROP

#include <queue>
#include <stack>
#include <utility>
#include <vector>

struct UnitClause {
	int clause_id;
	int literal;
	bool positive;
	explicit UnitClause(int c, int l)
	{
		clause_id = c;
		literal = l;
		positive = l > 0;
	}
};

class UnitPropagator {
	std::size_t _numvar;
	std::vector<std::vector<int>> _clauses;
	std::vector<bool> _is_defined;
	std::vector<bool> _value;
	std::vector<int> _head_index;
	std::vector<int> _tail_index;
	std::vector<std::vector<int>> _clauses_of_pos_head;
	std::vector<std::vector<int>> _clauses_of_neg_head;
	std::vector<std::vector<int>> _clauses_of_pos_tail;
	std::vector<std::vector<int>> _clauses_of_neg_tail;
	std::queue<int> _literal_queue;
	std::stack<UnitClause> _units;
	bool _OK;

	void insert_clause_head_list(int clause_id, int literal);
	void insert_clause_tail_list(int clause_id, int literal);
	void shorten_clause_from_head(int clause_id);
	void shorten_clause_from_tail(int clause_id);
	void propagate_true_value(int variable);
	void propagate_false_value(int variable);
	std::vector<std::vector<int>> produce_output();

public:
	UnitPropagator(const std::vector<std::vector<int>> &clauses, std::size_t numvar);
	std::pair<bool, std::vector<std::vector<int>>> propagate();

};


#endif