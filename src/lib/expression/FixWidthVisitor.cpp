#include "../../crave/expression/FixWidthVisitor.hpp"

#include <stdexcept>

namespace crave {

void FixWidthVisitor::visitNode(const Node&) { }

void FixWidthVisitor::visitTerminal(const Terminal&) { }

void FixWidthVisitor::visitUnaryExpr(const UnaryExpression& u)
{
  u.child()->visit(*this);
}

void FixWidthVisitor::visitUnaryOpr(const UnaryOperator&) { }

void FixWidthVisitor::visitBinaryExpr(const BinaryExpression& b)
{
  b.lhs()->visit(*this);
  b.rhs()->visit(*this);
}

void FixWidthVisitor::visitBinaryOpr(const BinaryOperator&)
{

}

void FixWidthVisitor::visitTernaryExpr(const TernaryExpression& t)
{
  t.a()->visit(*this);
  t.b()->visit(*this);
  t.c()->visit(*this);
}


void FixWidthVisitor::visitPlaceholder(const Placeholder& pl)
{
  exprStack_.push(std::make_pair(new Placeholder(pl), -1));
}

void FixWidthVisitor::visitVariableExpr(const VariableExpr& v) {
  exprStack_.push(std::make_pair(new VariableExpr(v), v.bitsize()));
}

void FixWidthVisitor::visitConstant(const Constant& c) {
  exprStack_.push(std::make_pair(new Constant(c), c.bitsize()));
}

void FixWidthVisitor::visitVectorExpr(const VectorExpr& v) {
  exprStack_.push(std::make_pair(new VectorExpr(v), v.bitsize()));
}

void FixWidthVisitor::visitNotOpr(const NotOpr& n)
{
  visitUnaryExpr(n);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(new NotOpr(e.first), e.second));
}

void FixWidthVisitor::visitNegOpr(const NegOpr& n)
{
  visitUnaryExpr(n);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(new NegOpr(e.first), e.second));
}

void FixWidthVisitor::visitComplementOpr(const ComplementOpr& c)
{
  visitUnaryExpr(c);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(new ComplementOpr(e.first), e.second));
}

void FixWidthVisitor::visitInside(const Inside& i)
{
  visitUnaryExpr(i);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(new Inside(e.first, i.collection()), e.second));
}

void FixWidthVisitor::visitExtendExpr(const ExtendExpression& e)
{
  visitUnaryExpr(e);

  stack_entry entry;
  pop(entry);

  exprStack_.push(std::make_pair(new ExtendExpression(entry.first, e.value()), entry.second));
}

void FixWidthVisitor::visitAndOpr(const AndOpr& a)
{
  stack_entry lhs, rhs;
  evalBinExpr(a, lhs, rhs);

  exprStack_.push(std::make_pair(new AndOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitOrOpr(const OrOpr& o)
{
  stack_entry lhs, rhs;
  evalBinExpr(o, lhs, rhs);

  exprStack_.push(std::make_pair(new OrOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitLogicalAndOpr(const LogicalAndOpr& la)
{
  stack_entry lhs, rhs;
  evalBinExpr(la, lhs, rhs);

  exprStack_.push(std::make_pair(new LogicalAndOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitLogicalOrOpr(const LogicalOrOpr& lo)
{
  stack_entry lhs, rhs;
  evalBinExpr(lo, lhs, rhs);

  exprStack_.push(std::make_pair(new LogicalAndOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitXorOpr(const XorOpr& x)
{
  stack_entry lhs, rhs;
  evalBinExpr(x, lhs, rhs);

  exprStack_.push(std::make_pair(new XorOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitEqualOpr(const EqualOpr& eq)
{
  stack_entry lhs, rhs;
  evalBinExpr(eq, lhs, rhs);

  exprStack_.push(std::make_pair(new EqualOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitNotEqualOpr(const NotEqualOpr& neq)
{
  stack_entry lhs, rhs;
  evalBinExpr(neq, lhs, rhs);

  exprStack_.push(std::make_pair(new NotEqualOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitLessOpr(const LessOpr& l)
{
  stack_entry lhs, rhs;
  evalBinExpr(l, lhs, rhs);

  exprStack_.push(std::make_pair(new LessOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitLessEqualOpr(const LessEqualOpr& le)
{
  stack_entry lhs, rhs;
  evalBinExpr(le, lhs, rhs);

  exprStack_.push(std::make_pair(new LessEqualOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitGreaterOpr(const GreaterOpr& g)
{
  stack_entry lhs, rhs;
  evalBinExpr(g, lhs, rhs);

  exprStack_.push(std::make_pair(new GreaterOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitGreaterEqualOpr(const GreaterEqualOpr& ge)
{
  stack_entry lhs, rhs;
  evalBinExpr(ge, lhs, rhs);

  exprStack_.push(std::make_pair(new GreaterEqualOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitPlusOpr(const PlusOpr& p)
{
  stack_entry lhs, rhs;
  evalBinExpr(p, lhs, rhs);

  exprStack_.push(std::make_pair(new PlusOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitMinusOpr(const MinusOpr& m)
{
  stack_entry lhs, rhs;
  evalBinExpr(m, lhs, rhs);

  exprStack_.push(std::make_pair(new MinusOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitMultipliesOpr(const MultipliesOpr& m)
{
  stack_entry lhs, rhs;
  evalBinExpr(m, lhs, rhs);

  exprStack_.push(std::make_pair(new MultipliesOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitDevideOpr(const DevideOpr& d)
{
  stack_entry lhs, rhs;
  evalBinExpr(d, lhs, rhs);

  exprStack_.push(std::make_pair(new DevideOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitModuloOpr(const ModuloOpr& m)
{
  stack_entry lhs, rhs;
  evalBinExpr(m, lhs, rhs);

  exprStack_.push(std::make_pair(new ModuloOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitShiftLeftOpr(const ShiftLeftOpr& shl)
{
  stack_entry lhs, rhs;
  evalBinExpr(shl, lhs, rhs);

  exprStack_.push(std::make_pair(new ShiftLeftOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitShiftRightOpr(const ShiftRightOpr& shr)
{
  stack_entry lhs, rhs;
  evalBinExpr(shr, lhs, rhs);

  exprStack_.push(std::make_pair(new ShiftRightOpr(lhs.first, rhs.first), lhs.second));
}

void FixWidthVisitor::visitVectorAccess(const VectorAccess& va) { throw std::runtime_error("VectorAccess is not allowed in FixWidthVisitor."); }
void FixWidthVisitor::visitForEach(const ForEach& va) { throw std::runtime_error("ForEach is not allowed in FixWidthVisitor."); }
void FixWidthVisitor::visitUnique(const Unique& va) { throw std::runtime_error("Unique is not allowed in FixWidthVisitor."); }

void FixWidthVisitor::visitIfThenElse(const IfThenElse& ite)
{
  stack_entry a, b, c;
  evalTernExpr(ite, a, b, c);

  exprStack_.push(std::make_pair(new IfThenElse(a.first, b.first, c.first), a.second));
}

void FixWidthVisitor::visitBitslice(const Bitslice& b)
{
  visitUnaryExpr(b);

  stack_entry e;
  pop(e);

  exprStack_.push(std::make_pair(new Bitslice(e.first, b.l(), b.r(), b.expr_size()), e.second));
}

} // end namespace crave
