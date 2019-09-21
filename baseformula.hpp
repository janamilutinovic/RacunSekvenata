#ifndef BASEFORMULA_H
#define BASEFORMULA_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>

using namespace std;

using Variable = string;
using FunctionSymbol = string;
using RelationSymbol = string;


class BaseTerm;
using Term = shared_ptr<BaseTerm>;


class BaseTerm : public enable_shared_from_this<BaseTerm> {

public:
  enum Type { VAR, FUNC };
  virtual Type getType() const = 0;
  virtual void printTerm(ostream &out) const = 0;
  virtual bool equalTo(const Term &oth) const = 0;

  virtual ~BaseTerm() {}
};

class VarTerm : public BaseTerm {
private:
  Variable v;
public:
  VarTerm(const Variable &vt)
    :v(vt)
  {}
  inline const Variable& variable() const { return v; }
  virtual Type getType() const
  {
    return VAR;
  }

  const Variable & getVariable() const
  {
    return v;
  }
  virtual void printTerm(ostream & out) const
  {
    out << v;
  }
  bool equalTo(const Term &oth) const
  {
      const VarTerm *pOth = dynamic_cast<const VarTerm*>(oth.get());
      if (pOth)
      {
          return pOth->variable() == variable();
      }
      else
      {
          return false;
      }
  }
};

class FunctionTerm : public BaseTerm {
private:
  FunctionSymbol f; //funkcijski simbol
  vector<Term> m_terms;

public:
  FunctionTerm(const FunctionSymbol &fs,
           const vector<Term> &terms = vector<Term> ())
    :f(fs),
     m_terms(terms)
  {}
   inline const FunctionSymbol& symbol() const { return f; }

  virtual Type getType() const
  {
    return FUNC;
  }

  const FunctionSymbol &getSymbol() const
  {
    return f;
  }

  const vector<Term> &getOperands() const
  {
    return m_terms;
  }

  virtual void printTerm(ostream &out) const
  {
      if (m_terms.empty())
      {
          out << f;
      }
      else
      {
          out << f << "(";
          for (auto first = m_terms.cbegin(), last = m_terms.cend(); first + 1 != last; ++first)
          {
              (*first)->printTerm(out);
              out << ", ";
          }
          m_terms.back()->printTerm(out);
          out << ")";
      }
  }
  bool equalTo(const Term &t) const
  {
    if(t->getType() != FUNC)
      return false;

    const FunctionTerm *pOth = dynamic_cast<const FunctionTerm*>(t.get());
    if (pOth)
    {
      return symbol() == pOth->symbol() &&
          m_terms.size() == pOth->m_terms.size() &&
          std::equal(m_terms.cbegin(), m_terms.cend(), pOth->m_terms.cbegin());
    }
    else
    {
      return false;
    }
  }
};

class BaseFormula;
using Formula = std::shared_ptr<BaseFormula>;

class BaseFormula : public enable_shared_from_this<BaseFormula> {

public:

  enum Type { TRUE, FALSE, ATOM, NOT, AND, OR, IMP, IFF ,FORALL, EXISTS};

  virtual void printFormula(ostream &out) const = 0;
  virtual Type getType() const = 0;
  virtual void getAtoms(std::vector<Formula> atoms) = 0;
  virtual int formulaDepth() const = 0;
  virtual ~BaseFormula() {}

  bool equalTo(const Formula &f) const
  {
      const BaseFormula *base = f.get();
      return typeid (*this) == typeid (*base);
  }
};



class AtomicFormula : public BaseFormula {
public:

};

class LogicConstant : public AtomicFormula {

public:
    bool equalTo( const Formula &f) const
    {
      return f->getType() == this->getType();
    }
    void getAtoms(std::vector<Formula> atoms){
        atoms.push_back(shared_from_this());
    }
    int formulaDepth() const{
        return 1;
    }
};


class True : public LogicConstant {

public:
  virtual void printFormula(ostream & ostr) const
  {
    ostr << "true";
  }

  virtual Type getType() const
  {
    return TRUE;
  }

};


class False : public LogicConstant {

public:
  virtual void printFormula(ostream & ostr) const
  {
    ostr << "false";
  }

  virtual Type getType() const
  {
    return FALSE;
  }
};


class Atom : public AtomicFormula {
protected:
  RelationSymbol rs;
  vector<Term> m_terms;

public:
  Atom(const RelationSymbol &r,
       const vector<Term> &terms = vector<Term>())
    :rs(r),
     m_terms(terms)
  {}

  inline const RelationSymbol& symbol() const { return rs; }

  const RelationSymbol & getSymbol() const
  {
    return rs;
  }

  const vector<Term> & getOperands() const
  {
    return m_terms;
  }

  virtual void printFormula(ostream & out) const
  {
      if (m_terms.empty())
      {
          out << rs;
      }
      else
      {
          out << rs << "(";
          for (auto first = m_terms.cbegin(), last = m_terms.cend(); first + 1 != last; ++first)
          {
              (*first)->printTerm(out);
              out << ", ";
          }
          m_terms.back()->printTerm(out);
          out << ")";
      }
  }

  virtual Type getType() const
  {
    return ATOM;
  }

  bool equalTo(const Formula &f) const
  {
      if(f->getType() != ATOM)
          return false;

      if(rs != ((Atom*) f.get())->getSymbol())
          return false;

      const Atom *pF = dynamic_cast<const Atom*>(f.get());
      if (pF)
      {
          return symbol() == pF->symbol() &&
                  m_terms.size() == pF->m_terms.size() &&
                  std::equal(m_terms.cbegin(), m_terms.cend(), pF->m_terms.cbegin());
      }
      else
      {
          return false;
      }
  }

  void getAtoms(std::vector<Formula> atoms){
      atoms.push_back(shared_from_this());
  }
  int formulaDepth() const {
    return 1;
  }
};

class Equality : public Atom {
public:
  Equality(const Term & lop, const Term & rop)
    :Atom("=", vector<Term> ())
  {
    m_terms.push_back(lop);
    m_terms.push_back(rop);
  }

  const Term & getLeftOperand() const
  {
    return m_terms[0];
  }

  const Term & getRightOperand() const
  {
    return m_terms[1];
  }

  virtual void printFormula(ostream & out) const
  {
    m_terms[0]->printTerm(out);
    out << " = ";
    m_terms[1]->printTerm(out);
  }
};

class Disequality : public Atom {
public:
  Disequality(const Term & lop, const Term & rop)
    :Atom("~=", vector<Term> ())
  {
    m_terms.push_back(lop);
    m_terms.push_back(rop);
  }

  const Term & getLeftOperand() const
  {
    return m_terms[0];
  }

  const Term & getRightOperand() const
  {
    return m_terms[1];
  }

  virtual void printFormula(ostream & out) const
  {

    m_terms[0]->printTerm(out);
    out << " ~= ";
    m_terms[1]->printTerm(out);
  }
};


class UnaryConnective : public BaseFormula {
protected:
   Formula _f;
public:
  UnaryConnective(const Formula &f)
    :_f(f)
  {}

  const Formula &getOperand() const
  {
    return _f;
  }
};

class Not : public UnaryConnective {
public:
  Not(const Formula &f)
    :UnaryConnective(f)
  {}

  virtual void printFormula(ostream &out) const
  {
    out << "~";
    Type op_type = _f->getType();

    if(op_type == AND || op_type == OR ||
       op_type == IMP || op_type == IFF)
      out << "(";

    _f->printFormula(out);

    if(op_type == AND || op_type == OR ||
       op_type == IMP || op_type == IFF)
      out << ")";
  }

  virtual Type getType() const
  {
    return NOT;
  }
  bool equalTo(const Formula &f) const
  {
    return f->getType() == this->getType() &&
      f->equalTo(((UnaryConnective *)f.get())->getOperand());
  }
  void getAtoms(std::vector<Formula> atoms){
     _f->getAtoms(atoms);
  }
  int formulaDepth() const {
      return _f->formulaDepth() + 1;
  }
};


class BinaryConnective : public BaseFormula {
protected:
   Formula f1,f2;
public:
  BinaryConnective(const Formula &f_1, const Formula &f_2)
    :f1(f_1),
     f2(f_2)
  {}

  const Formula & getOperand1() const
  {
    return f1;
  }

  const Formula & getOperand2() const
  {
    return f2;
  }

  bool equalTo(const Formula &f) const
  {
    if (BaseFormula::equalTo(f))
    {
      return f1->equalTo(static_cast<const BinaryConnective*>(f.get())->f1) &&
          f2->equalTo(static_cast<const BinaryConnective*>(f.get())->f2);
    }
    return false;
  }

  void getAtoms(std::vector<Formula> atoms){
     f1->getAtoms(atoms);
     f2->getAtoms(atoms);
  }
  int formulaDepth() const{
      if(f1->formulaDepth() > f2->formulaDepth()){
          return f1->formulaDepth() + 1;
      }
      else{
          return f2->formulaDepth() + 1;
      }
  }
};


class And : public BinaryConnective {
public:
  And(const Formula &f1, const Formula &f2)
    :BinaryConnective(f1, f2)
  {}

  virtual void printFormula(ostream & out) const
  {
    Type f1_type = f1->getType();
    Type f2_type = f2->getType();

    if(f1_type == OR || f1_type == IMP ||
       f1_type == IFF)
      out << "(";

    f1->printFormula(out);

    if(f1_type == OR || f1_type == IMP ||
       f1_type == IFF)
      out << ")";

    out << " & ";

    if(f2_type == OR || f2_type == IMP ||
       f2_type == IFF || f2_type == AND)
      out << "(";

    f2->printFormula(out);

    if(f2_type == OR || f2_type == IMP ||
       f2_type == IFF || f2_type == AND)
      out << ")";
  }

  virtual Type getType() const
  {
    return AND;
  }
 };



class Or : public BinaryConnective {
public:
  Or(const Formula &f1, const Formula &f2)
    :BinaryConnective(f1, f2)
  {}

  virtual void printFormula(ostream & out) const
  {

    Type f1_type = f1->getType();
    Type f2_type = f2->getType();

    if(f1_type == IMP || f1_type == IFF)
      out << "(";

    f1->printFormula(out);

    if(f1_type == IMP || f1_type == IFF)
      out << ")";

    out << " | ";

    if(f2_type == IMP ||
       f2_type == IFF || f2_type == OR)
      out << "(";

    f2->printFormula(out);

    if(f2_type == IMP ||
       f2_type == IFF || f2_type == OR)
      out << ")";
  }

  virtual Type getType() const
  {
    return OR;
  }
};


class Imp : public BinaryConnective {
public:
  Imp(const Formula & op1, const Formula & op2)
    :BinaryConnective(op1, op2)
  {}

  virtual void printFormula(ostream & out) const
  {

    Type f1_type = f1->getType();
    Type f2_type = f2->getType();

    if(f1_type == IFF)
      out << "(";

    f1->printFormula(out);

    if(f1_type == IFF)
      out << ")";

    out << " => ";

    if(f2_type == IMP || f2_type == IFF)
      out << "(";

    f2->printFormula(out);

    if(f2_type == IMP || f2_type == IFF)
      out << ")";
  }

  virtual Type getType() const
  {
    return IMP;
  }
};



class Iff : public BinaryConnective {

public:
  Iff(const Formula & op1, const Formula & op2)
    :BinaryConnective(op1, op2)
  {}

  virtual void printFormula(ostream & out) const
  {

    Type f1_type = f1->getType();
    Type f2_type = f2->getType();

    f1->printFormula(out);

    out << " => ";

    if(f2_type == IFF)
      out << "(";

    f2->printFormula(out);

    if(f2_type == IFF)
      out << ")";

  }

  virtual Type getType() const
  {
    return IFF;
  }
};


class Quantifier : public BaseFormula {
protected:
  Variable _v;
  Formula  _op;
public:
  Quantifier(const Variable & v, const Formula & op)
    :_v(v),
     _op(op)
  {}

  const Variable & getVariable() const
  {
    return _v;
  }

  const Formula & getOperand() const
  {
    return _op;
  }
  bool equalTo(const Formula & f) const
  {
    return f->getType() == getType() &&
      ((Quantifier *) f.get())->getVariable() == _v &&
      ((Quantifier *) f.get())->getOperand()->equalTo(_op);
  }
  void getAtoms(std::vector<Formula> atoms){
     _op->getAtoms(atoms);
  }
  int formulaDepth() const{
      return 0;
  }
};

class Forall : public Quantifier {
public:
  Forall(const Variable & v, const Formula & op)
    :Quantifier(v, op)
  {}

  virtual Type getType() const
  {
    return FORALL;
  }
  virtual void printFormula(ostream & out) const
  {
    cout << "![" << _v << "] : ";

    Type op_type = _op->getType();

    if(op_type == AND || op_type == OR ||
       op_type == IMP || op_type == IFF)
      out << "(";

    _op->printFormula(out);

    if(op_type == AND || op_type == OR ||
       op_type == IMP || op_type == IFF)
      out << ")";
  }
};

class Exists : public Quantifier {
public:
  Exists(const Variable & v, const Formula & op)
    :Quantifier(v, op)
  {}

  virtual Type getType() const
  {
    return EXISTS;
  }

  virtual void printFormula(ostream & out) const
  {
    cout  << "?[" << _v << "] : ";

    Type op_type = _op->getType();

    if(op_type == AND || op_type == OR ||
       op_type == IMP || op_type == IFF)
      out << "(";

    _op->printFormula(out);

    if(op_type == AND || op_type == OR ||
       op_type == IMP || op_type == IFF)
      out << ")";
  }
};

inline
ostream &operator << (ostream & out, const Term &t)
{
  t->printTerm(out);
}

inline
ostream & operator << (ostream & out, const Formula &f)
{
  f->printFormula(out);
}

extern Formula input_formula;


#endif // _FOL_H
