#ifndef SIMULATORWINDOW_H
#define SIMULATORWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>

namespace Ui { class SimulatorWindow; }

class SimulatorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SimulatorWindow(QWidget *parent = nullptr);
    ~SimulatorWindow();

private slots:
    void on_pushButtonSzukaj_clicked();
    void on_pushButtonPolacz_clicked();
    void on_pushButtonRozlacz_clicked();
    void on_btnStartStop_clicked();

    void readFromPort();
    void onTimerInterrupt();
    void onErrorOccurred(QSerialPort::SerialPortError error);

private:
    Ui::SimulatorWindow *ui;
    QSerialPort *device;
    QTimer *timer;
    bool isRunning;
    unsigned char StanWyjsc;

    void addToLogs(QString message);
};
#endif
