#ifndef _IO_UTILS_H
#define _IO_UTILS_H 1

#include <cstdlib>

#include <fstream>
#include <string>
#include <initializer_list>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

template <typename ISTR, typename OSTR, typename ERRSTR>
class dimacsIO
{
    public:
        dimacsIO(ISTR& _istr, OSTR& _ostr, ERRSTR& _errstr) : istr(_istr), ostr(_ostr), errstr(_errstr)  {}

        void printComment(const std::string& comments) const
        {
            ostr << "c " << comments << std::endl;
        }

        template <typename Val>
            void printResult(const Val& val) const {
                ostr << "v " << val << std::endl;
            }

        template <typename Context, typename Formula>
            void read(Context& ctx, Formula& formula) const
            {
                char op;
                std::string s1, s2;
                while (istr >> op)
                {
                    switch (op)
                    {
                        case 'c':
                            getline(istr, s1);
#ifdef VERBOSE
                            printComment(s1);
#endif
                            break;
                        case 'p':
                            istr >> s1 >> ctx.numVars >> ctx.numClauses;
#ifdef VERBOSE
                            printComment(" solving " + s1 + " formula with " +
                                    std::to_string(ctx.numVars) + " variables and " + std::to_string(ctx.numClauses) + " caluses");
#endif
                            formula.resize(ctx.numClauses);
                            readFormula<Context>(ctx, formula); return;
                            break;
                        default:
                            errstr << "[ ERROR ] Bad format!!!\n";
                            exit(1);
                    }
                }
            }
        ISTR& in() { return istr; }
        OSTR& out() { return ostr; }
        ERRSTR& err() { return errstr; }

    private:
        template <typename Context, typename Formula>
            void readFormula(Context& ctx, Formula& formula) const
            {
                typedef typename Context::literal_type literal_type;

                literal_type v1;
                for (uint32_t idx = 0; idx < ctx.numClauses; ++idx)
                {
                    std::vector<literal_type> clause;
                    while (istr >> v1)
                    {
                        if (!v1) break;
                        clause.push_back(v1);
                    }
                    formula[idx].createClause(std::move(clause));
                }
            }

        ISTR& istr;
        OSTR& ostr;
        ERRSTR& errstr;
};

typedef dimacsIO<std::istream, std::ostream, std::ostream> cdimacsIO;

#endif // _IO_UTILS_H
