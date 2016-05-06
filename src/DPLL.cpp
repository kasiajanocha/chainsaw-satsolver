#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <tuple>
#include <utility>
#include <vector>

#include "unit_propagation.h"

std::pair<bool, std::vector<std::vector<int>>> unit_propagation(const std::vector<std::vector<int>> &clauses, std::size_t numvar)
{
	return UnitPropagator(clauses, numvar).propagate();
}

bool solve_sat(std::size_t numvar, std::vector<std::vector<int>> &clauses, std::vector<int> &assignments)
{
	if(numvar == assignments.size())
		return true;

	assignments.emplace_back(1);
	clauses.emplace_back(std::initializer_list<int>{static_cast<int>(assignments.size())});

	bool possible;
	std::vector<std::vector<int>> up_clauses;

	std::tie(possible, up_clauses) = unit_propagation(clauses, numvar);

	if(possible && solve_sat(numvar, up_clauses, assignments))
		return true;

	assignments.back() = -1;
	clauses.back().front() = -static_cast<int>(assignments.size());

	std::tie(possible, up_clauses) = unit_propagation(clauses, numvar);

	if(possible && solve_sat(numvar, up_clauses, assignments))
		return true;

	assignments.pop_back();
	return false;
}

void solve(std::size_t numvar, std::vector<std::vector<int>> &clauses)
{
	std::vector<int> assignments;
	bool possible;
	std::vector<std::vector<int>> up_clauses;

	std::tie(possible, up_clauses) = unit_propagation(clauses, numvar);

	if(possible && solve_sat(numvar, up_clauses, assignments))
	{
		std::printf("s SATISFIABLE\n");

		for(std::size_t i = 0; i < numvar; ++i)
		{
			std::printf("v ");

			if(std::signbit(assignments[i]))
				std::printf("-");

			std::printf("%zu\n", i + 1);
		}
	}
	else
		std::printf("s UNSATISFIABLE\n");
}

int main()
{
	char line[1024], format[256];
	std::size_t numvar, numclause;
	std::vector<std::vector<int>> clauses;

	while(true)
	{
		if(std::fgets(line, sizeof(line), stdin) == nullptr)
			break;

		if(line[0] != 'p')
			continue;

		if(std::sscanf(line, "p %s%zu%zu", format, &numvar, &numclause) != 3)
		{
			std::printf("Bad input line: %s\n", line);
			return -1;
		}

		if(std::strcmp(format, "cnf") != 0)
		{
			std::printf("Bad format: %s\n", format);
			return -1;
		}

		for(std::size_t i = 0; i < numclause; ++i)
		{
			std::vector<int> clause;

			while(true)
			{
				int var;

				if(std::scanf("%d", &var) != 1)
				{
					std::printf("Bad input\n");
					return -1;
				}

				if(var == 0)
					break;

				clause.emplace_back(var);
			}

			clauses.emplace_back(std::move(clause));
		}
	}

	solve(numvar, clauses);
	return 0;
}
