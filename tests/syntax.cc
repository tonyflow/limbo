// vim:filetype=cpp:textwidth=120:shiftwidth=2:softtabstop=2:expandtab
// Copyright 2014--2016 Christoph Schwering

#include <gtest/gtest.h>

#include <lela/formula.h>
#include <lela/format/syntax.h>
#include <lela/format/output.h>

namespace lela {

using namespace lela::format;

#define REGISTER_SYMBOL(x)    RegisterSymbol(x, #x)

inline void RegisterSymbol(Term t, const std::string& n) {
  RegisterSymbol(t.symbol(), n);
}

TEST(InputTest, general) {
  Symbol::Factory sf;
  Term::Factory tf;
  Context ctx(&sf, &tf);
  auto Bool = ctx.NewSort();
  auto True = ctx.NewName(Bool);            REGISTER_SYMBOL(True);
  auto HUMAN = ctx.NewSort();
  auto Father = ctx.NewFun(HUMAN, 1);       REGISTER_SYMBOL(Father);
  auto Mother = ctx.NewFun(HUMAN, 1);       REGISTER_SYMBOL(Mother);
  auto IsParentOf = ctx.NewFun(Bool, 2);    REGISTER_SYMBOL(IsParentOf);
  auto John = ctx.NewFun(Bool, 0);          REGISTER_SYMBOL(John);
  auto x = ctx.NewVar(HUMAN);               REGISTER_SYMBOL(x);
  auto y = ctx.NewVar(HUMAN);               REGISTER_SYMBOL(y);
  {
    Formula phi = Ex(x, John() == x);
    EXPECT_EQ(phi, Formula::Exists(x, Formula::Clause(Clause{Literal::Eq(tf.CreateTerm(John, {}), x)})));
    EXPECT_EQ(phi.reader().NF(), Formula::Exists(x, Formula::Clause(Clause{Literal::Eq(tf.CreateTerm(John, {}), x)})));
  }
  {
    Formula phi = Fa(x, John() == x);
    EXPECT_EQ(phi, Formula::Not(Formula::Exists(x, Formula::Not(Formula::Clause(Clause{Literal::Eq(tf.CreateTerm(John, {}), x)})))));
    EXPECT_EQ(phi.reader().NF(), Formula::Not(Formula::Exists(x, Formula::Clause(Clause{Literal::Neq(tf.CreateTerm(John, {}), x)}))));
  }
  {
    Formula phi = Fa(x, IsParentOf(Mother(x), x) == True && IsParentOf(Father(x), x) == True);
    EXPECT_EQ(phi, Formula::Not(Formula::Exists(x, Formula::Not(Formula::Not(Formula::Or(Formula::Not(Formula::Clause(Clause{Literal::Eq(tf.CreateTerm(IsParentOf, {tf.CreateTerm(Mother, {x}), x}), True)})),
                                                                                         Formula::Not(Formula::Clause(Clause{Literal::Eq(tf.CreateTerm(IsParentOf, {tf.CreateTerm(Father, {x}), x}), True)}))))))));
    EXPECT_EQ(phi.reader().NF(), Formula::Not(Formula::Exists(x, Formula::Clause(Clause{Literal::Neq(tf.CreateTerm(IsParentOf, {tf.CreateTerm(Mother, {x}), x}), True),
                                                                                        Literal::Neq(tf.CreateTerm(IsParentOf, {tf.CreateTerm(Father, {x}), x}), True)}))));
  }
  {
    Formula phi = Fa(x, IsParentOf(x, y) == True && IsParentOf(Father(x), x) == True);
    EXPECT_EQ(phi, Formula::Not(Formula::Exists(x, Formula::Not(Formula::Not(Formula::Or(Formula::Not(Formula::Clause(Clause{Literal::Eq(tf.CreateTerm(IsParentOf, {x, y}), True)})),
                                                                                         Formula::Not(Formula::Clause(Clause{Literal::Eq(tf.CreateTerm(IsParentOf, {tf.CreateTerm(Father, {x}), x}), True)}))))))));
    EXPECT_EQ(phi.reader().NF(), Formula::Not(Formula::Exists(x, Formula::Clause(Clause{Literal::Neq(tf.CreateTerm(IsParentOf, {x, y}), True),
                                                                                        Literal::Neq(tf.CreateTerm(IsParentOf, {tf.CreateTerm(Father, {x}), x}), True)}))));
  }

  {
    auto P = ctx.NewFun(Bool, 1);    REGISTER_SYMBOL(P);
    auto Q = ctx.NewFun(Bool, 1);    REGISTER_SYMBOL(P);
    // That's the example formula from my thesis.
    Formula phi = Ex(x, P(x) == True) >> Fa(y, Q(y) == True);
    EXPECT_EQ(phi, Formula::Or(Formula::Not(Formula::Exists(x, Formula::Clause(Clause{Literal::Eq(tf.CreateTerm(P, {x}), True)}))),
                               Formula::Not(Formula::Exists(y, Formula::Not(Formula::Clause(Clause{Literal::Eq(tf.CreateTerm(Q, {y}), True)}))))));
    EXPECT_EQ(phi.reader().NF(), Formula::Not(Formula::Exists(x, Formula::Not(Formula::Not(Formula::Exists(y, Formula::Not(
                                  Formula::Clause(Clause{Literal::Neq(tf.CreateTerm(P, {x}), True),
                                                         Literal::Eq(tf.CreateTerm(Q, {y}), True)})
                                )))))));
  }
}

}  // namespace lela

