#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <queue>
#include <stack>
#include <tuple>
#include <utility>
#include <vector>

#include "unit_propagation.h"
#include "sat_utils.h"

/**
 * Implementation of a linear unit propagation described in this paper:
 * http://www.cfdvs.iitb.ac.in/download/Docs/verification/papers/sat/original-papers/aim96.pdf
 * working in amortized linear time for true assignments and amortized constant time for false assignments.
 */

UnitPropagator::UnitPropagator(const std::vector<std::vector<int>> &clauses, std::size_t numvar)
{
	_clauses = clauses;
	_numvar = numvar;
	_is_defined = std::vector<bool>(numvar, false);
	_value = std::vector<bool>(numvar, false);
	_OK = true;
	_clauses_of_pos_head = std::vector<std::vector<int>>(numvar, std::vector<int>());
	_clauses_of_neg_head = std::vector<std::vector<int>>(numvar, std::vector<int>());
	_clauses_of_pos_tail = std::vector<std::vector<int>>(numvar, std::vector<int>());
	_clauses_of_neg_tail = std::vector<std::vector<int>>(numvar, std::vector<int>());

	_head_index = std::vector<int>(_clauses.size(), 0);
	for (int i = 0; i < _clauses.size(); ++i)
		_tail_index.push_back((int)(_clauses[i].end() - _clauses[i].begin()) - 1);
}

void UnitPropagator::insert_clause_head_list(int clause_id, int literal)
{
	if (literal > 0)
		_clauses_of_pos_head[std::abs(literal)].push_back(clause_id);
	else
		_clauses_of_neg_head[std::abs(literal)].push_back(clause_id);
}

void UnitPropagator::insert_clause_tail_list(int clause_id, int literal)
{
	if (literal > 0)
		_clauses_of_pos_tail[std::abs(literal)].push_back(clause_id);
	else
		_clauses_of_neg_tail[std::abs(literal)].push_back(clause_id);
}

void UnitPropagator::shorten_clause_from_head(int clause_id)
{
	for (int i = _head_index[clause_id] + 1; i <= _tail_index[clause_id]; ++i)
	{
		int literal = _clauses[clause_id][i];
		if (!_is_defined[std::abs(literal)])
		{
			if (_tail_index[clause_id] == i)
				_units.push(UnitClause(clause_id, literal));
			else insert_clause_head_list(clause_id, literal);
		}
		else if (_value[std::abs(literal)]) return;
	}
	// there is an empty clause
	_OK = false;
}

// analogous to shorten_clause_from_head
void UnitPropagator::shorten_clause_from_tail(int clause_id)
{
	for (int i = _tail_index[clause_id] - 1; i >= _head_index[clause_id]; --i)
	{
		int literal = _clauses[clause_id][i];
		if (!_is_defined[std::abs(literal)])
		{
			if (_head_index[clause_id] == i)
				_units.push(UnitClause(clause_id, literal));
			else insert_clause_tail_list(clause_id, literal);
		}
		else if (_value[std::abs(literal)]) return;
	}
	_OK = false;
}

void UnitPropagator::propagate_true_value(int variable)
{
	for (int clause_id : _clauses_of_neg_head[variable])
		if(_OK)
			shorten_clause_from_head(clause_id);

	for (int clause_id : _clauses_of_neg_tail[variable])
		if(_OK)
			shorten_clause_from_tail(clause_id);
}

void UnitPropagator::propagate_false_value(int variable)
{
	for (int clause_id : _clauses_of_pos_head[variable])
		if(_OK)
			shorten_clause_from_head(clause_id);

	for (int clause_id : _clauses_of_pos_tail[variable])
		if(_OK)
			shorten_clause_from_tail(clause_id);
}

std::vector<std::vector<int>> UnitPropagator::produce_output()
{
	// TODO: return clauses with applied unit propagation.
	return _clauses;
}

std::pair<bool, std::vector<std::vector<int>>> UnitPropagator::propagate()
{
	for (int c = 0; c < _clauses.size(); ++c)
	{
		if (_head_index[c] == _tail_index[c])
			_units.push(UnitClause(c, _clauses[c][_head_index[c]]));
	}

	while(_OK && !_units.empty())
	{
		UnitClause L = _units.top();
		_units.pop();
		if (_is_defined[std::abs(L.literal)] && _value[std::abs(L.literal)])
			continue;
		else
		{
			if (_is_defined[std::abs(L.literal)] && !_value[std::abs(L.literal)])
			{
				_OK = false;
				break;
			}
			else
			{
				_is_defined[std::abs(L.literal)] = true;
				if (L.positive)
				{
					_value[std::abs(L.literal)] = true;
					propagate_true_value(std::abs(L.literal));
				}
				else
				{
					_value[std::abs(L.literal)] = false;
					propagate_false_value(std::abs(L.literal));
				}
			}
		}
	}

	return std::make_pair(_OK, produce_output());
}