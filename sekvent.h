#ifndef SEKVENT_H
#define SEKVENT_H

#include <QVector>
#include "baseformula.hpp"
#include "map"

class Sekvent
{
private:
    QVector<Formula> levi;
    QVector<Formula> desni;
    bool obradjen;
    int id;
   // int m_parent_id;

public:
    Sekvent(QVector<Formula> levi,QVector<Formula> desni, bool obr, int id);
    Sekvent();
    QString print_Sekvent();
    QVector<Formula> getLeft();
    QVector<Formula> getRight();
    //int getId();
    //int getParentId();
    std::map<string,int> checkRules();
    bool getObradjen();
    void setObradjen(bool o);
    bool equalTo(Sekvent s1);
    int getId();

};

#endif // SEKVENT_H
