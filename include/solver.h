#ifndef _SOLVER_H
#define _SOLVER_H

#include "ioutils.h"
#include "unit_propagator.h"
#include "sat_utils.h"

#include <cassert>

#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <queue>

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
        ctx.initialise(&formula);
        for (idx_type idx = 0; idx < ctx.numClauses; ++idx)
            for (const auto& lit : formula[idx].data)
            {
                if (lit > 0) ctx.positive_occur[static_cast<idx_type>(lit)].push_back(idx);
                else ctx.negative_occur[static_cast<idx_type>(-lit)].push_back(idx);
            }
        feeder.reset(new LiteralFeeder(ctx));
    }

    bool solutionFound() {
        for (auto c  : formula)
            if (!c.isSatisfied()) return false;
        return true;
    }

    void fillValuationWithTrue() {
        for (int i = 0; i<ctx.valuation.size(); i++)
            if (ctx.valuation[i] == UNASSIGNED)
                ctx.valuation[i] = TRUE;
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
        return res;
    }

    Result solve(int level=0) {
        if (find_if(formula.begin(), formula.end(), [](const ClauseType& cl) { return cl.isEmpty(); }) != formula.end())
           return UNSATISFIED;

        if (!unitPropagate(level)) return UNSATISFIED;
        if (solutionFound()) {
            fillValuationWithTrue();
            return SATISFIED;
        }

        literal_type literal = feeder->getLiteral();
        if (literal == 0) return SATISFIED;

        TriBool val = feeder->getValuation(literal);
        ctx.valuation[literal] = val;
        io.out() << "setting " << literal << " => "  << (val ? "TRUE" : "FALSE") << std::endl;

        const auto& occursPos = ( val ? ctx.positive_occur[literal] : ctx.negative_occur[literal]);
        const auto& occursNeg = (!val ? ctx.positive_occur[literal] : ctx.negative_occur[literal]);
        for (const auto& idx : occursPos)
        {
            formula[idx].resolved++;
            formula[idx].positive++;
        }
        for (const auto& idx : occursNeg)
            formula[idx].resolved++;

        Result res = solve(level+1);
        if (!res)
        {
            ctx.valuation[literal] = (val ? FALSE : TRUE);
            io.out() << "setting " << literal << " => "  << (!val ? "TRUE" : "FALSE") << std::endl;
            for (const auto& idx : occursPos) formula[idx].positive--;
            for (const auto& idx : occursNeg) formula[idx].positive++;
            res = solve(level+1);
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

    bool unitPropagate(int level) {
        return UnitPropagator<IO, ClauseType, ValuationType, LiteralFeeder>(ctx, io, formula, *feeder.get()).propagate(level);
    }

    computation_context<ClauseType, ValuationType> ctx;
    std::unique_ptr<LiteralFeeder> feeder;
    IO& io;
    std::vector<ClauseType> formula;
};

#endif // _SOLVER_H
