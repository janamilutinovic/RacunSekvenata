#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "baseformula.hpp"
#include <sstream>
#include<qstring.h>
#include <qmessagebox.h>
#include "sekvent.h"
#include <set>
#include "qdebug.h"


using namespace std;

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string(const char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern void yy_switch_to_buffer (YY_BUFFER_STATE new_buffer  );

extern Formula input_formula;
QVector<Sekvent> sekventi = {};
std::vector<Sekvent> clanovi = {}; // u ovom stablu ce se cuvati veza izmedju parent i child sekven(a)ta
Sekvent currsek;
//QString separator =

void MainWindow::reset()
{
    ui->textEdit->clear();
    ui->preostali->clear();
    sekventi.clear();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Dokazivač teorema iskazne logike - racun sekvenata");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_prove_clicked()
{
    reset();
    QString formula = ui->inputFormula->text();
    formula += ";";
    string inputFormula = formula.toUtf8().constData();

    YY_BUFFER_STATE buffer = yy_scan_string(inputFormula.c_str());
    if(yyparse() == 1){
        QMessageBox msgBox;
        msgBox.setText("Neispravna formula na ulazu");

    }

    QVector<Formula> right;
    right.append(Formula(input_formula));
    QVector<Formula> left = {};
    Sekvent sek(left,right, false,0);

    qDebug() << sek.print_Sekvent() ;
    qDebug() << sek.getObradjen() ;

    sekventi.append(sek);
    clanovi.push_back(sek);

    ui->ass->setEnabled(true);
    ui->weakL->setEnabled(true);
    ui->weakR->setEnabled(true);
    ui->textEdit->append(sek.print_Sekvent());

   std::map<string,int> tipoviFla = sek.checkRules();

    enableRules(tipoviFla);
    sekventi[0].setObradjen(true);

    currsek = sek;

}

void MainWindow::enableRules(std::map<string,int> tipovi)
{
    ui->notR->setDisabled(true);
    ui->notL->setDisabled(true);
    ui->andL->setDisabled(true);
    ui->andR->setDisabled(true);
    ui->orR->setDisabled(true);
    ui->orL->setDisabled(true);
    ui->impL->setDisabled(true);
    ui->impR->setDisabled(true);


    for(int i =0; i< tipovi.size(); i++)
    {
        if(tipovi["r"]==BaseFormula::NOT) {
            ui->notR->setEnabled(true);
        }
        else if(tipovi["l"]==BaseFormula::NOT) {
            ui->notL->setEnabled(true);
        }
        else if(tipovi["r"]==BaseFormula::AND) {
            ui->andR->setEnabled(true);
        }
        else if(tipovi["l"]==BaseFormula::AND) {
            ui->andL->setEnabled(true);
        }
        else if(tipovi["r"]==BaseFormula::OR) {
            ui->orR->setEnabled(true);
        }
        else if(tipovi["l"]==BaseFormula::OR) {
            ui->orL->setEnabled(true);
        }
        else if(tipovi["r"]==BaseFormula::IMP) {
            ui->impR->setEnabled(true);
        }
        else if(tipovi["l"]==BaseFormula::IMP) {
            ui->impL->setEnabled(true);
        }
    }
}

Sekvent& MainWindow::getNextSeq()
{
    for(int i=0;i<sekventi.length();i++)
    {
        qDebug() << sekventi[i].print_Sekvent() << "   " <<sekventi[i].getObradjen();
    }

    for(int i=0;i<sekventi.length();i++)
    {
        if(!sekventi[i].getObradjen()) {
            return sekventi[i];
        }
    }
    return currsek;

}


Sekvent MainWindow::currSeqv(Sekvent currentSekvent){

    Sekvent cs ;
    int i=0;
    for(;i < sekventi.length(); i++) {
        if(sekventi[i].print_Sekvent() == currentSekvent.print_Sekvent()) {
            cs = sekventi[i];
        }
    }

    return cs;
}


void MainWindow::on_andR_clicked()
{
   Sekvent curr = currsek;
   QVector<Formula> leftSekvent = curr.getLeft();
   QVector<Formula> rightSekvent = curr.getRight();

   int i;

   for (i=0; i<rightSekvent.size(); i++) {

       if (rightSekvent[i]->getType() == BaseFormula::AND) {
           Formula leftFormula = ((And*)(rightSekvent[i].get()))->getOperand1();
           Formula rightFormula = ((And*)(rightSekvent[i].get()))->getOperand2();

           QVector<Formula> newRightSekvent1 = rightSekvent;
           newRightSekvent1.remove(i);
           newRightSekvent1.append(rightFormula);


           QVector<Formula> newRightSekvent2 = rightSekvent;
           newRightSekvent2.remove(i);
           newRightSekvent2.append(leftFormula);



           Sekvent newSekvent1(leftSekvent,newRightSekvent1, false, sekventi.length());
           Sekvent newSekvent2(leftSekvent,newRightSekvent2, false, sekventi.length()+1);


           QString oldtext = ui->textEdit->toPlainText();

           ui->textEdit->setPlainText(newSekvent1.print_Sekvent() +"\t"+ newSekvent2.print_Sekvent());
           ui->textEdit->append(oldtext);

           sekventi.push_back(newSekvent1);
           sekventi.push_back(newSekvent2);
           preostalo();

           currsek = newSekvent1;
           sekventi[newSekvent1.getId()].setObradjen(true);


       }

   }

    qDebug() << "sledeci";
   qDebug() << currsek.print_Sekvent();
              //qDebug() << current_Sekvent;
   enableRules(currsek.checkRules());
   //Sekvent Sekvent = Sekvents.at(current_Sekvent)

}




void MainWindow::on_ass_clicked()
{

    Sekvent curr = currsek;
    qDebug() << curr.print_Sekvent();

    //curr.setObradjen(true);
    QVector<Formula> leftSekvent = curr.getLeft();
    QVector<Formula> rightSekvent = curr.getRight();
    int i,j;
    bool ass = false;

    for (i=0; i<leftSekvent.size(); i++) {
        for (j=0; j<rightSekvent.size(); j++) {
            qDebug() << "da li su isti? " << leftSekvent[i]->equalTo(rightSekvent[j]);

            if (leftSekvent[i].operator=(rightSekvent[j])) {
                qDebug() << "isti" << leftSekvent[i].get() << rightSekvent[i].get();
                QString oldtext = ui->textEdit->toPlainText();
                ui->textEdit->clear();
                ui->textEdit->setPlainText("ass");
                ui->textEdit->append(oldtext);
                ass = true;
                qDebug() << currsek.getId() << curr.getId();
                sekventi[curr.getId()].setObradjen(true);
                currsek.setObradjen(true);
                break;

            }
        }
    }
    qDebug() << "curr" << currsek.print_Sekvent() <<currsek.getId();
    Sekvent &nextsek = this->getNextSeq();
    qDebug() << "next" << nextsek.print_Sekvent() << nextsek.getId();
    qDebug() << (currsek.equalTo(nextsek) && nextsek.getObradjen());
    if(ass && currsek.equalTo(nextsek) && nextsek.getObradjen())
    {

        QString oldtext = ui->textEdit->toPlainText();
        ui->textEdit->clear();
        ui->textEdit->setPlainText("TEOREMA");
        ui->textEdit->append(oldtext);
        preostalo();
    }

    sekventi[nextsek.getId()].setObradjen(true);
    currsek = nextsek;
    enableRules(currsek.checkRules());

}


void MainWindow::on_impR_clicked()
{

    Sekvent curr = currsek;
    curr.setObradjen(true);
    QVector<Formula> leftSekvent = curr.getLeft();
    QVector<Formula> rightSekvent = curr.getRight();

        int i;
        for (i=0; i<rightSekvent.size(); i++) {

            if (rightSekvent[i].get()->getType() == 6) {
                Formula leftFormula = ((Imp*)(rightSekvent[i].get()))->getOperand1();
                Formula rightFormula = ((Imp*)(rightSekvent[i].get()))->getOperand2();


                QVector<Formula> newrightSekvent = rightSekvent;
                newrightSekvent.replace(i, rightFormula);

                QVector<Formula> newLeftSequent = leftSekvent;
                newLeftSequent.append(leftFormula);

                Sekvent novi(newLeftSequent,newrightSekvent, true, sekventi.length());
                sekventi.push_back(novi);
                preostalo();

                QString oldtext = ui->textEdit->toPlainText();

                ui->textEdit->setPlainText(novi.print_Sekvent());
                ui->textEdit->append(oldtext);

                currsek = novi;

                qDebug() << "Primenjeno pravilo impRight, current je ";
                qDebug() << currsek.print_Sekvent();
            }
        }

 enableRules(currsek.checkRules());
}

void MainWindow::updateSekventi(Sekvent curr)
{
    //for (int i=0; i< sekventi.length();i++)
     //   if (sekventi[i]->equalTo(curr) && !sekventi[i]->getObradjen())
       //     sekventi[i]->setObradjen(true);
}

void MainWindow::preostalo()
{
    ui->preostali->clear();
    for (int i=0; i< sekventi.length();i++)
       {
        qDebug() << "Sek " << sekventi[i].print_Sekvent();
        if (!sekventi[i].getObradjen())
            this->ui->preostali->append(sekventi[i].print_Sekvent());
    }
    }


void MainWindow::on_andL_clicked()
{
    Sekvent curr = currsek;
    qDebug() << "andL " << curr.print_Sekvent();
    QVector<Formula> leftSekvent = curr.getLeft();
    QVector<Formula> rightSekvent = curr.getRight();
    qDebug() << leftSekvent[0].get()->getType();
    //qDebug() << rightSekvent;

    int i;

    for (i=0; i<leftSekvent.size(); i++) {

        if (leftSekvent[i]->getType() == BaseFormula::AND) {
            qDebug() << "ok";
            Formula leftFormula = ((And*)(leftSekvent[i].get()))->getOperand1();
            Formula rightFormula = ((And*)(leftSekvent[i].get()))->getOperand2();

            QVector<Formula> newRightSekvent1 = leftSekvent;
            newRightSekvent1.remove(i);
            newRightSekvent1.append(rightFormula);


            QVector<Formula> noviLevi = leftSekvent;
            noviLevi.remove(i);
            noviLevi.append(leftFormula);
            noviLevi.append(rightFormula);
            Sekvent novi(noviLevi,rightSekvent, true,sekventi.length());
            qDebug() << novi.print_Sekvent();
            QString oldtext = ui->textEdit->toPlainText();

            ui->textEdit->setPlainText(novi.print_Sekvent() );
            ui->textEdit->append(oldtext);

            sekventi.push_back(novi);
            preostalo();

            currsek = novi;


 }
    }


    enableRules(currsek.checkRules());

}

void MainWindow::on_weakL_clicked()
{
    Sekvent curr = currsek;
    QVector<Formula> leftSekvent = curr.getLeft();
    QVector<Formula> rightSekvent = curr.getRight();
    int n = leftSekvent.size()-1;

    QVector<Formula> noviLevi = leftSekvent;
    noviLevi.remove(n);

    Sekvent novi(noviLevi,rightSekvent, true,sekventi.length());

    QString oldtext = ui->textEdit->toPlainText();

    ui->textEdit->setPlainText(novi.print_Sekvent() );
    ui->textEdit->append(oldtext);

    sekventi.push_back(novi);
    preostalo();
    sekventi[novi.getId()].setObradjen(true);
    currsek = novi;
}

void MainWindow::on_weakR_clicked()
{
    Sekvent curr = currsek;
    QVector<Formula> leftSekvent = curr.getLeft();
    QVector<Formula> rightSekvent = curr.getRight();
    int n = rightSekvent.size()-1;

    QVector<Formula> noviDesni = rightSekvent;
    noviDesni.remove(n);

    Sekvent novi(leftSekvent,noviDesni, true,sekventi.length());

    QString oldtext = ui->textEdit->toPlainText();

    ui->textEdit->setPlainText(novi.print_Sekvent() );
    ui->textEdit->append(oldtext);

    sekventi.push_back(novi);
    preostalo();
    sekventi[novi.getId()].setObradjen(true);
    currsek = novi;
}


