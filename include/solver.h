#ifndef _SOLVER_H
#define _SOLVER_H

#include "ioutils.h"

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
        valuation()
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
};

enum Result
{
    UNSATISFIED = 0,
    SATISFIED,
    UNKNOWN,
};

template <typename IO, typename ClauseType, typename ValuationType, typename LiteralFeeder>
class solver
{
    typedef typename ClauseType::literal_type literal_type;
    public:
    solver(IO& _io) :
        ctx(),
        feeder(nullptr),
        io(_io),
        formula()
    {
    }

    void fillFormula() {
        io.read(ctx, formula);
        ctx.initialise();
        for (idx_type idx = 0; idx < ctx.numClauses; ++idx)
            for (const auto& lit : formula[idx].data)
            {
                if (lit > 0) ctx.positive_occur[static_cast<idx_type>(lit)].push_back(idx);
                else ctx.negative_occur[static_cast<idx_type>(-lit)].push_back(idx);
            }
        feeder.reset(new LiteralFeeder(ctx));
    }

    Result run() {
        Result res = solve();
        if (res == SATISFIED)
        {
            if (verify())
                io.out() << "s SATISFIED " << std::endl << "v " << ctx.valuation << std::endl;
            else
                io.out() << "s UNKNOWN" << std::endl;
        }
        else
            io.out() << "s UNSATISFIED" << std::endl;
    }

    Result solve() {
        if (find_if(formula.begin(), formula.end(), [](const ClauseType& cl) { return cl.isEmpty(); }) != formula.end())
           return UNSATISFIED;

        unitPropagate();

        literal_type literal = feeder->getLiteral();
        if (literal == 0) return SATISFIED;

        TriBool val = feeder->getValuation(literal);
        ctx.valuation[literal] = val;
        // io.out() << "setting " << literal << " => "  << (val ? "TRUE" : "FALSE") << std::endl;

        const auto& occursPos = ( val ? ctx.positive_occur[literal] : ctx.negative_occur[literal]);
        const auto& occursNeg = (!val ? ctx.positive_occur[literal] : ctx.negative_occur[literal]);
        for (const auto& idx : occursPos)
        {
            formula[idx].resolved++;
            formula[idx].positive++;
        }
        for (const auto& idx : occursNeg)
            formula[idx].resolved++;

        Result res = solve();
        if (!res)
        {
            ctx.valuation[literal] = (val ? FALSE : TRUE);
            // io.out() << "setting " << literal << " => "  << (!val ? "TRUE" : "FALSE") << std::endl;
            for (const auto& idx : occursPos) formula[idx].positive--;
            for (const auto& idx : occursNeg) formula[idx].positive++;
            res = solve();
            for (const auto& idx : occursNeg) formula[idx].positive--;
        }
        else
            for (const auto& idx : occursPos) formula[idx].positive++;

        for (const auto& idx : occursPos) formula[idx].resolved--;
        for (const auto& idx : occursNeg) formula[idx].resolved--;

        feeder->takeBackLiteral(literal);
        return res;
    }

    bool verify() {
        for (const auto& cl : formula)
        {
            bool sat = false;
            for (const auto& l : cl.data)
            {
                if ((POS(l) && ctx.valuation[l] == TRUE) ||
                    (NEG(l) && ctx.valuation[-l] == FALSE)) sat = true;
                if (sat) break;
            }
            if (!sat) return false;
        }
        return true;
    }

    void unitPropagate(){}

    computation_context<ClauseType, ValuationType> ctx;
    std::unique_ptr<LiteralFeeder> feeder;
    IO& io;
    std::vector<ClauseType> formula;
};

#endif // _SOLVER_H
