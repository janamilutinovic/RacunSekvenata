#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "baseformula.hpp"
#include "sekvent.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void enableRules(std::map<string,int> tipovi);
    Sekvent currSeqv(Sekvent currentSequent);
    Sekvent &getNextSeq();
    void updateSekventi(Sekvent curr);
    void preostalo();
    void reset();

private slots:
    void on_prove_clicked();
    void on_andR_clicked();
    void on_ass_clicked();
    void on_impR_clicked();
    void on_andL_clicked();
    void on_weakL_clicked();
    void on_weakR_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
