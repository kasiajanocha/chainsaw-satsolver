#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <utility>
#include <vector>

constexpr int var_index(int var)
{
	return std::abs(var) - 1;
}

bool is_model(const std::vector<std::vector<int>> &clauses, std::vector<int> &assignments)
{
	return std::all_of(clauses.cbegin(), clauses.cend(), [&assignments](const std::vector<int> &clause) -> bool {
		return std::any_of(clause.cbegin(), clause.cend(), [&assignments](int var) -> bool {
			return var * assignments[var_index(var)] > 0;
		});
	});
}

bool solve_sat(std::size_t numvar, const std::vector<std::vector<int>> &clauses, std::vector<int> &assignments)
{
	if(numvar == assignments.size())
		return is_model(clauses, assignments);

	assignments.emplace_back(1);

	if(solve_sat(numvar, clauses, assignments))
		return true;

	assignments.pop_back();
	assignments.emplace_back(-1);

	if(solve_sat(numvar, clauses, assignments))
		return true;

	assignments.pop_back();
	return false;
}

void solve(std::size_t numvar, const std::vector<std::vector<int>> &clauses)
{
	std::vector<int> assignments;

	if(solve_sat(numvar, clauses, assignments))
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
