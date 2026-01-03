#include "mainwindow.h"
#include "simulatorwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w_panel;
    w_panel.setWindowTitle("Urzadzenie 1");
    w_panel.move(100, 100);
    w_panel.show();

    SimulatorWindow w_sim;
    w_sim.setWindowTitle("Urzadzenie 2");
    w_sim.move(800, 100);
    w_sim.show();

    return a.exec();
}
