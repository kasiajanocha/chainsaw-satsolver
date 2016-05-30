#ifndef _FEEDER_H
#define _FEEDER_H

#include <ctime>
#include <cstdlib>

#include <algorithm>
#include <numeric>
#include <vector>

#include "solver.h"

template <typename ComputationContext>
class simpleLiteralFeeder
{
    public:
    typedef typename ComputationContext::literal_type literal_type;

    simpleLiteralFeeder(ComputationContext& _ctx) :
        idx(1),
        varBound(_ctx.numVars)
    {
    }

    literal_type getLiteral() {
        return (varBound >= idx) ? idx++ : 0;
    }

    TriBool getValuation(const literal_type) const {
        return TRUE;
    }

    void takeBackLiteral(const literal_type lit) {
        idx = lit;
    }

    literal_type idx;
    literal_type varBound;
};

template <typename ComputationContext>
class randomizedLiteralFeeder
{
    public:
    typedef typename ComputationContext::literal_type literal_type;

    randomizedLiteralFeeder(ComputationContext& _ctx) :
        idx(0),
        valuation(_ctx.numVars),
        literalOrdering(_ctx.numVars)
    {
        srand(time(nullptr));
        literal_type jdx = 1;
        for (auto& ll : literalOrdering)
            ll = jdx++;
        random_shuffle(literalOrdering.begin(), literalOrdering.end());
        for (auto& vl : valuation)
            vl = (random() & 1 ? TRUE : FALSE);
    }

    literal_type getLiteral() {
        return (idx < literalOrdering.size()) ? literalOrdering[idx++] : 0;
    }

    TriBool getValuation(const literal_type l) const {
        return valuation[l - 1];
    }

    void takeBackLiteral(const literal_type) {
        --idx;
    }

    idx_type idx;
    std::vector<TriBool> valuation;
    std::vector<literal_type> literalOrdering;
};

#endif // _FEEDER_H
