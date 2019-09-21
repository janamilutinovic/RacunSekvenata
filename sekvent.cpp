#include "sekvent.h"
#include <sstream>
#include <qdebug.h>
#include <QString>
#include "set"


Sekvent::Sekvent(QVector<Formula> levi, QVector<Formula> desni, bool obr, int id)//, int id, int parent_id)
{
    this->levi = levi;
    this->desni = desni;
   // this->m_id = id;
   // this->m_parent_id = parent_id;
    this->obradjen = obr;
    this->id = id;
}

Sekvent::Sekvent () {

}

//int Sekvent::getId() {
//    return m_id;
//}

//int Sekvent::getParentId() {
//    return m_parent_id;
//}

QVector<Formula> Sekvent::getLeft(){
    return this->levi;
}

QVector<Formula> Sekvent::getRight(){
    return this->desni;
}

QString Sekvent::print_Sekvent(){
    int i;
    int n = getLeft().length();
    int n1 = getRight().length();
    std::ostringstream stream;

    if (n == 0) {
        stream <<" ";
    }

    if(n == 1){
        for (i = 0; i < n; i++){
            getLeft()[i]->printFormula(stream);
        }
    }
    else if(n == 2){
            getLeft()[0]->printFormula(stream);
            stream << ',';
            getLeft()[1]->printFormula(stream);
    }

    else if (n > 2){
        for (i = 0; i <= n-2; i++){
            getLeft()[i]->printFormula(stream);
            stream << ", ";
        }
        getLeft()[n-1]->printFormula(stream);
    }

    stream << "|- ";

    if (n1 == 0) {
        stream<< " ";
    }

    if(n1 == 1){
        for (i = 0; i < n1; i++){
            getRight()[i]->printFormula(stream);
        }
    }
    else if(n1 == 2){
            getRight()[0]->printFormula(stream);
            stream << ',';
            getRight()[1]->printFormula(stream);
    }

    else if (n1 > 2){
        for (i = 0; i <= n1-2; i++){
            getRight()[i]->printFormula(stream);
            stream << ", ";
        }
        getRight()[n1-1]->printFormula(stream);
    }


    string formulaStream = stream.str();
    return QString::fromStdString(formulaStream);

}

std::map<string,int> Sekvent::checkRules()
{
    std::map<string,int> tipovi;
    QVector<Formula>::iterator it;
    for(it = this->getLeft().begin(); it != this->getLeft().end(); ++it)
    {
        int tip = it->get()->getType();
        tipovi["l"]=tip;
    }

    for(it = this->getRight().begin(); it != this->getRight().end(); ++it)
    {
        int tip = it->get()->getType();
        tipovi["r"]=tip;
    }
    return tipovi;
}

void Sekvent::setObradjen(bool o)
{
    this->obradjen = o;
}

bool Sekvent::getObradjen()
{
    return this->obradjen;
}

int Sekvent::getId()
{
    return  this->id;
}

bool Sekvent::equalTo(Sekvent s1)
{
    if (this->getLeft()==s1.getLeft())
        if(this->getRight()==s1.getRight())
            return true;
    return false;
}


