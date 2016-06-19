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

    void reportConflict(const std::vector<literal_type> &c) {
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

    void reportConflict(const std::vector<literal_type> &c) {
    }

    idx_type idx;
    std::vector<TriBool> valuation;
    std::vector<literal_type> literalOrdering;
};

template <typename ComputationContext>
class vsidsLiteralFeeder
{
    public:
    typedef typename ComputationContext::literal_type literal_type;
    typedef float ctr_type;
    static constexpr float conflict_div = 1.05;


    vsidsLiteralFeeder(ComputationContext& _ctx):
        heap_size(_ctx.numVars),
        valuation(_ctx.numVars + 1),
        heap(_ctx.numVars),
        counter(_ctx.numVars + 1, 0)
    {
        srand(time(nullptr));
        for (auto& vl : valuation)
            vl = (random() & 1 ? TRUE : FALSE);

        for(auto &c : *_ctx.formula)
            for(auto &l : c.data)
                ++counter[std::abs(l)];

	for(std::size_t i = 1; i <= _ctx.numVars; ++i)
	    heap[i - 1] = std::make_pair(counter[i], i);

        std::make_heap(heap.begin(), heap.begin() + heap_size);
    }

    literal_type getLiteral() {
        if(heap_size == 0)
            return 0;

        literal_type l = heap[0].second;
        std::pop_heap(heap.begin(), heap.begin() + (heap_size--));
        return l;
    }

    TriBool getValuation(const literal_type l) const {
        return valuation[l - 1];
    }

    void takeBackLiteral(const literal_type l) {
        heap[heap_size] = std::make_pair(counter[l], l);
	std::push_heap(heap.begin(), heap.begin() + (++heap_size));
    }

    void reportConflict(const std::vector<literal_type> &c) {
	for(auto &c: counter)
            c /= conflict_div;
        for(std::size_t i = 0; i < heap_size; ++i)
        {
            heap[i].first /= conflict_div;
            const literal_type l = heap[i].second;
            if(std::any_of(c.begin(), c.end(), [l](literal_type c) { return l == std::abs(c); }))
            {
                heap[i].first += 1;
                for(std::size_t j = i; j > 0; j /= 2)
                    if(heap[j] > heap[j / 2])
                        std::swap(heap[j], heap[j / 2]);
                    else
                        break;
            }
        }
    }

    std::vector<TriBool> valuation;
    std::vector<std::pair<ctr_type, idx_type>> heap;
    std::vector<ctr_type> counter;
    std::size_t heap_size;
};

#endif // _FEEDER_H
