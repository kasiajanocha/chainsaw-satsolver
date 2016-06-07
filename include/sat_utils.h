#ifndef _SAT_UTILS_H
#define _SAT_UTILS_H

#include "ioutils.h"
#include "unit_propagator.h"

#include <cassert>

#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <queue>

#define DEFAULT_CLAUSE_SIZE 3

#define POS(x) ((x) > 0)
#define NEG(x) ((x) < 0)

enum TriBool
{
    FALSE = 0,
    TRUE,
    UNASSIGNED
};

typedef uint32_t idx_type;

template <typename _Boolean>        
class valuation : public std::vector<_Boolean>
{
    typedef std::vector<_Boolean> Base;
public:
    typedef _Boolean Boolean;

    template <typename T>
    friend std::ostream& operator<< (std::ostream&, const valuation<T>&);
};

template <typename T>
std::ostream& operator<< (std::ostream& ostr, const valuation<T>& val)
{
    for (idx_type idx = 1; idx <= val.size(); ++idx)
    {
        ostr << (val[idx] ? " " : " -") << idx;
    }
    return ostr;
}

template <typename Literal=int16_t>
class _clause
{
    public:
    typedef Literal literal_type;
    typedef typename std::vector<literal_type>::iterator iterator;

    _clause() :
        positive(0), resolved(0), data()
    {
    }

    void createClause(std::vector<literal_type>&& _data) {
        data = _data;
        done();
    }

    inline bool isSatisfied() const {
        return positive > 0;
    }

    inline bool isUnitClause() const {
        return ((data.size() - resolved) == 1) && !isSatisfied();
    }

    bool isEmpty() const {
        return (resolved == data.size()) && !isSatisfied();
    }

    void done() {
        auto end = std::unique(data.begin(), data.end());
        data.erase(end, data.end());
        data.shrink_to_fit();
    }

    uint8_t positive;
    uint8_t resolved;
    std::vector<literal_type> data;
};

typedef _clause<int16_t> clause;

template <typename ClauseType, typename ValuationType>
class computation_context
{
    public:
    typedef ClauseType clause_type;
    typedef typename clause::literal_type literal_type;

    typedef ValuationType valuation_type;
    typedef typename valuation_type::Boolean Boolean;

    computation_context() :
        numVars(0),
        numClauses(0),
        positive_occur(),
        negative_occur(),
        valuation(),
        resolved_literals_by_level()
    {
    }

    void initialise()
    {
        valuation.resize(numVars + 1);
        for (auto& v : valuation) v = UNASSIGNED;
        positive_occur.resize(valuation.size());
        negative_occur.resize(valuation.size());
    }

    literal_type numVars;
    uint32_t numClauses;
    std::vector<std::vector<idx_type>> positive_occur;
    std::vector<std::vector<idx_type>> negative_occur;
    valuation_type valuation;
    std::vector<std::vector<literal_type>> resolved_literals_by_level;
};

enum Result
{
    UNSATISFIED = 0,
    SATISFIED,
    UNKNOWN,
};

#endif // _SAT_UTILS_H