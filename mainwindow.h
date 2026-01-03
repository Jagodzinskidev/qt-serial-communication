#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonSzukaj_clicked();
    void on_pushButtonPolacz_clicked();
    void on_pushButtonRozlacz_clicked();

    void on_checkBox_stateChanged(int arg1);
    void on_checkBox_2_stateChanged(int arg1);
    void on_checkBox_3_stateChanged(int arg1);
    void on_checkBox_4_stateChanged(int arg1);
    void on_checkBox_8_stateChanged(int arg1);
    void on_checkBox_5_stateChanged(int arg1);
    void on_checkBox_6_stateChanged(int arg1);
    void on_checkBox_7_stateChanged(int arg1);

    void readFromPort();
    void onErrorOccurred(QSerialPort::SerialPortError error);
    void onTimerInterrupt();
    void onPlotTimerTick();

private:
    Ui::MainWindow *ui;
    QSerialPort *device;
    QTimer *timer;
    QTimer *plotTimer;

    unsigned char StanWejsc;
    unsigned int AC1Wyjscie;
    unsigned int AC2Wyjscie;
    unsigned int AC3Wyjscie;

    double freq;
    double amp;
    double bias;
    int shape;
    bool run;
    double plotTime;

    void addToLogs(QString message);
    void updateBit(int bitMask, int state);
};
#endif
