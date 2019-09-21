#include "baseformula.hpp"

BaseFormula::BaseFormula()
{

}


void Atom::printFormula(std::ostream &out) const
{
    std::cout << "p" << m_p;
}

void Not::printFormula(std::ostream &out) const
{
    std::cout << "~";
    m_f->printFormula(out);
}

void And::printFormula(std::ostream &out) const
{
    std::cout << "(";
    m_f1->printFormula(out);
    std::cout << "/\\";
    m_f2->printFormula(out);
    std::cout << ")";
}

void Or::printFormula(std::ostream &out) const
{
    std::cout << "(";
    m_f1->printFormula(out);
    std::cout << "\\/";
    m_f2->printFormula(out);
    std::cout << ")";
}


void Imp::printFormula(std::ostream &out) const
{
    std::cout << "(";
    m_f1->printFormula(out);
    std::cout << "=>";
    m_f2->printFormula(out);
    std::cout << ")";
}

void Eqv::printFormula(std::ostream &out) const
{
    std::cout << "(";
    m_f1->printFormula(out);
    std::cout << "<=>";
    m_f2->printFormula(out);
    std::cout << ")";
}

