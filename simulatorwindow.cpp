#include "simulatorwindow.h"
#include "ui_simulatorwindow.h"
#include <QSerialPortInfo>
#include <QDateTime>

SimulatorWindow::SimulatorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SimulatorWindow)
    , device(new QSerialPort(this))
    , timer(new QTimer(this))
    , isRunning(false)
    , StanWyjsc(0)
{
    ui->setupUi(this);

    connect(timer, &QTimer::timeout, this, &SimulatorWindow::onTimerInterrupt);
    connect(device, &QSerialPort::readyRead, this, &SimulatorWindow::readFromPort);
    connect(device, &QSerialPort::errorOccurred, this, &SimulatorWindow::onErrorOccurred);
}

SimulatorWindow::~SimulatorWindow()
{
    delete ui;
}

void SimulatorWindow::addToLogs(QString message){
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
    ui->textEdit->append(currentDateTime + "\t" + message);
}

void SimulatorWindow::on_pushButtonSzukaj_clicked()
{
    QList<QSerialPortInfo> devices;
    devices  = QSerialPortInfo::availablePorts();
    for (int i = 0; i < devices.length(); i++){
        ui->comboBox->addItem(devices.at(i).portName()+ "\t" + devices.at(i).description());
        this->addToLogs("Znaleziono urzadzenie " + devices.at(i).portName()+ "\t" + devices.at(i).description());
    }
}

void SimulatorWindow::on_pushButtonPolacz_clicked()
{
    if(ui->comboBox->count() == 0) {
        this->addToLogs("Nie wykryto żadnych urządzeń!");
        return;
    }

    QString portName = ui->comboBox->currentText().split("\t").first();
    this->device->setPortName(portName);

    if(device->open(QSerialPort::ReadWrite)) {
        this->device->setBaudRate(QSerialPort::Baud9600);
        this->device->setDataBits(QSerialPort::Data8);
        this->device->setParity(QSerialPort::NoParity);
        this->device->setStopBits(QSerialPort::OneStop);
        this->device->setFlowControl(QSerialPort::NoFlowControl);
        this->addToLogs("Otwarto port szeregowy.");
    } else {
        this->addToLogs("Otwarcie portu szeregowego się nie powiodło!");
    }
}

void SimulatorWindow::on_pushButtonRozlacz_clicked()
{
    this->timer->stop();
    if(this->device->isOpen()) {
        this->device->close();
        this->addToLogs("Zamknięto połączenie.");
    } else {
        this->addToLogs("Port nie jest otwarty!");
        return;
    }
}

void SimulatorWindow::on_btnStartStop_clicked()
{
    if(isRunning){
        isRunning = false;
        ui->btnStartStop->setText("START");
        addToLogs("Wykres STOP");
    } else {
        isRunning = true;
        ui->btnStartStop->setText("STOP");
        if(!timer->isActive() && device->isOpen()) {
            timer->start(1000);
        }
        addToLogs("Wykres START");
    }
}

void SimulatorWindow::onTimerInterrupt()
{
    double freq = ui->FreqDial->value() / 10.0;
    double amp  = ui->AmpDial->value()  / 10.0;
    double bias = ui->BiasDial->value() / 10.0;
    int shape   = ui->SignalType->currentIndex();
    int runStatus = isRunning ? 1 : 0;
    if(device->isOpen() && device->isWritable()){
        QString data = QString("P=%1,%2,%3,%4,%5\n")
            .arg(freq)
            .arg(amp)
            .arg(bias)
            .arg(shape)
            .arg(runStatus);
        device->write(data.toLocal8Bit());
    }
}

void SimulatorWindow::readFromPort() {
    while(this->device->canReadLine()) {
        QString line = this->device->readLine();
        QString terminator = "\n";
        int pos = line.lastIndexOf(terminator);
        this->addToLogs(line.left(pos));
        QString str= line.left(pos);
        if(str.contains("IN=")){
            QString dane = str.split("=").last();
            if(!dane.isEmpty()){
                QList<QString> lista = dane.split(",");
                if(lista.length() == 4){
                    int l1=lista.at(0).toInt();
                    if(l1 >= 0 && l1 <256){
                        StanWyjsc = 0;
                        unsigned char pom = l1;
                        if(pom&0x80){
                            StanWyjsc = StanWyjsc+128;
                            ui->label_7->setText("1");
                        }
                        else ui->label_7->setText("0");
                        if(pom&0x40){
                            StanWyjsc = StanWyjsc+64;
                            ui->label_6->setText("1");
                        }
                        else ui->label_6->setText("0");
                        if(pom&0x20){
                            StanWyjsc = StanWyjsc+32;
                            ui->label_5->setText("1");
                        }
                        else ui->label_5->setText("0");
                        if(pom&0x10){
                            StanWyjsc = StanWyjsc+16;
                            ui->label_4->setText("1");
                        }
                        else ui->label_4->setText("0");
                        if(pom&0x08){
                            StanWyjsc = StanWyjsc+8;
                            ui->label_3->setText("1");
                        }
                        else ui->label_3->setText("0");
                        if(pom&0x04){
                            StanWyjsc = StanWyjsc+4;
                            ui->label_2->setText("1");
                        }
                        else ui->label_2->setText("0");
                        if(pom&0x02){
                            StanWyjsc = StanWyjsc+2;
                            ui->label_1->setText("1");
                        }
                        else ui->label_1->setText("0");
                        if(pom&0x01){
                            StanWyjsc = StanWyjsc+1;
                            ui->label_0->setText("1");
                        }
                        else ui->label_0->setText("0");
                        ui->StanWyjsc->setText(QString::number(StanWyjsc));
                    }
                    int l2 = lista.at(1).toInt();
                    if(l2 >= 0 && l2 < 1024){
                        l2 =(int) ((l2*100)/1023);
                        ui->AC1->setValue(l2);
                    }
                    int l3 = lista.at(2).toInt();
                    if(l3 >= 0 && l3 < 1024){
                        l3 = (int) ((l3*100)/1023);
                        ui->AC2->setValue(l3);
                    }
                    int l4 = lista.at(3).toInt();
                    if(l4 >= 0 && l4 < 1024){
                        l4 = (int) ((l4*100)/1023);
                        ui->AC3->setValue(l4);
                    }
                }
            }


        }
    }
}
void SimulatorWindow::onErrorOccurred(QSerialPort::SerialPortError error){
    if(error != QSerialPort::NoError){
        addToLogs("Błąd portu: " + device->errorString());
    }
}
