#ifndef _UNIT_PROP
#define _UNIT_PROP

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <queue>
#include <stack>
#include <tuple>
#include <utility>
#include <vector>

#include "ioutils.h"
#include "sat_utils.h"

/**
 * Implementation of a linear unit propagation described in this paper:
 * http://www.cfdvs.iitb.ac.in/download/Docs/verification/papers/sat/original-papers/aim96.pdf
 * working in amortized linear time for true assignments and amortized constant time for false assignments.
 */

struct UnitClause {
	int clause_id;
	int literal;
	bool positive;
	TriBool desired;
	explicit UnitClause(int c, int l)
	{
		clause_id = c;
		literal = l;
		positive = l > 0;
		if(positive) desired = TRUE;
		else desired = FALSE;
	}
};

template <typename IO, typename ClauseType, typename ValuationType>
class UnitPropagator {
	computation_context<ClauseType, ValuationType>& _ctx;
	std::size_t _numclauses;
	std::vector<std::vector<int>> _clauses;
	std::vector<int> _head_index;
	std::vector<int> _tail_index;
	std::vector<std::vector<int>> _clauses_of_pos_head;
	std::vector<std::vector<int>> _clauses_of_neg_head;
	std::vector<std::vector<int>> _clauses_of_pos_tail;
	std::vector<std::vector<int>> _clauses_of_neg_tail;
	std::queue<int> _literal_queue;
	std::stack<UnitClause> _units;
	bool _OK;
	IO _io;
	std::vector<ClauseType>& _formula;

	void insert_clause_head_list(int clause_id, int literal);
	void insert_clause_tail_list(int clause_id, int literal);
	void shorten_clause_from_head(int clause_id);
	void shorten_clause_from_tail(int clause_id);
	void propagate_true_value(int variable);
	void propagate_false_value(int variable);

public:
	typedef typename clause::literal_type literal_type;
	UnitPropagator(computation_context<ClauseType, ValuationType>& ctx, IO& io, std::vector<ClauseType>& formula);
	bool propagate(int level);
};

template <typename IO, typename ClauseType, typename ValuationType>
UnitPropagator<IO, ClauseType, ValuationType>::UnitPropagator(computation_context <ClauseType, ValuationType>& ctx, IO& io, std::vector<ClauseType>& formula):
	_io(io),
	_ctx(ctx),
	_formula(formula)
{
	_clauses = std::vector<std::vector<int>>(_ctx.numClauses, std::vector<int>());

	_OK = true;
	_clauses_of_pos_head = std::vector<std::vector<int>>(_ctx.numVars + 1, std::vector<int>());
	_clauses_of_neg_head = std::vector<std::vector<int>>(_ctx.numVars + 1, std::vector<int>());
	_clauses_of_pos_tail = std::vector<std::vector<int>>(_ctx.numVars + 1, std::vector<int>());
	_clauses_of_neg_tail = std::vector<std::vector<int>>(_ctx.numVars + 1, std::vector<int>());

	_head_index = std::vector<int>(ctx.numClauses + 1, 0);
	for (int i = 0; i < _clauses.size(); ++i)
		_tail_index.push_back((int)(_clauses[i].end() - _clauses[i].begin()) - 1);
}

template <typename IO, typename ClauseType, typename ValuationType>
void UnitPropagator<IO, ClauseType, ValuationType>::insert_clause_head_list(int clause_id, int literal)
{
	if (literal > 0)
		_clauses_of_pos_head[std::abs(literal)].push_back(clause_id);
	else
		_clauses_of_neg_head[std::abs(literal)].push_back(clause_id);
}

template <typename IO, typename ClauseType, typename ValuationType>
void UnitPropagator<IO, ClauseType, ValuationType>::insert_clause_tail_list(int clause_id, int literal)
{
	if (literal > 0)
		_clauses_of_pos_tail[std::abs(literal)].push_back(clause_id);
	else
		_clauses_of_neg_tail[std::abs(literal)].push_back(clause_id);
}

template <typename IO, typename ClauseType, typename ValuationType>
void UnitPropagator<IO, ClauseType, ValuationType>::shorten_clause_from_head(int clause_id)
{
	for (int i = _head_index[clause_id] + 1; i <= _tail_index[clause_id]; ++i)
	{
		int literal = _clauses[clause_id][i];
		if (_ctx.valuation[std::abs(literal)] == UNASSIGNED)
		{
			if (_tail_index[clause_id] == i)
				_units.push(UnitClause(clause_id, literal));
			else insert_clause_head_list(clause_id, literal);
		}
		else if (_ctx.valuation[std::abs(literal)]==TRUE) return;
	}
	// there is an empty clause
	_OK = false;
}

// analogous to shorten_clause_from_head
template <typename IO, typename ClauseType, typename ValuationType>
void UnitPropagator<IO, ClauseType, ValuationType>::shorten_clause_from_tail(int clause_id)
{
	for (int i = _tail_index[clause_id] - 1; i >= _head_index[clause_id]; --i)
	{
		int literal = _clauses[clause_id][i];
		if (_ctx.valuation[std::abs(literal)] == UNASSIGNED)
		{
			if (_head_index[clause_id] == i)
				_units.push(UnitClause(clause_id, literal));
			else insert_clause_tail_list(clause_id, literal);
		}
		else if (_ctx.valuation[std::abs(literal)]==TRUE) return;
	}
	_OK = false;
}

template <typename IO, typename ClauseType, typename ValuationType>
void UnitPropagator<IO, ClauseType, ValuationType>::propagate_true_value(int variable)
{
	for (int clause_id : _clauses_of_neg_head[variable])
		if(_OK && !_formula[clause_id].isSatisfied())
			shorten_clause_from_head(clause_id);

	for (int clause_id : _clauses_of_neg_tail[variable])
		if(_OK && !_formula[clause_id].isSatisfied())
			shorten_clause_from_tail(clause_id);
}

template <typename IO, typename ClauseType, typename ValuationType>
void UnitPropagator<IO, ClauseType, ValuationType>::propagate_false_value(int variable)
{
	for (int clause_id : _clauses_of_pos_head[variable])
		if(_OK && !_formula[clause_id].isSatisfied())
			shorten_clause_from_head(clause_id);

	for (int clause_id : _clauses_of_pos_tail[variable])
		if(_OK && !_formula[clause_id].isSatisfied())
			shorten_clause_from_tail(clause_id);
}

template <typename IO, typename ClauseType, typename ValuationType>
bool UnitPropagator<IO, ClauseType, ValuationType>::propagate(int level)
{
	// initializing clauses only if we are actually going to perform unit propagation
	bool contains_unit_clause = false;
	for (auto c : _formula) if (c.isUnitClause()) contains_unit_clause  = true;
	if(!contains_unit_clause) return true;

	for (int c = 0; c < _formula.size(); c++)
		for(int l = 0 ; l < _formula[c].data.size(); l++)
			if (_ctx.valuation[std::abs(_formula[c].data[l])] == UNASSIGNED)
				_clauses[c].push_back(_formula[c].data[l]);

	for (int c = 0; c < _clauses.size(); ++c)
	{
		if (_head_index[c] == _tail_index[c] && _formula[c].isUnitClause())
			_units.push(UnitClause(c, _clauses[c][_head_index[c]]));
	}

	while(_OK && !_units.empty())
	{
		UnitClause L = _units.top();
		_units.pop();
		if (_ctx.valuation[std::abs(L.literal)]==L.desired) // klauzula jest spelniona
			continue;
		else
		{
			if (_ctx.valuation[std::abs(L.literal)]!=UNASSIGNED && _ctx.valuation[std::abs(L.literal)]!=L.desired)
			{
				_OK = false;
				break;
			}
			else
			{
				while (_ctx.resolved_literals_by_level.size() < level+1) _ctx.resolved_literals_by_level.push_back(std::vector<literal_type>());
				_ctx.resolved_literals_by_level[level].push_back(L.literal);
				if (L.positive)
				{
					_ctx.valuation[std::abs(L.literal)] = TRUE;
					propagate_true_value(std::abs(L.literal));
					_formula[L.clause_id].resolved++;
					_formula[L.clause_id].positive++;
				}
				else
				{
					_ctx.valuation[std::abs(L.literal)] = FALSE;
					propagate_false_value(std::abs(L.literal));
					_formula[L.clause_id].resolved++;
				}
			}
		}
	}

	return _OK;
}

#endif