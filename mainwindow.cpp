#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QDateTime>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , device(new QSerialPort(this))
    , timer(new QTimer(this))
    , plotTimer(new QTimer(this))
    , StanWejsc(0)
    , freq(1.0), amp(0.0), bias(0.0), shape(0)
    , run(false), plotTime(0.0)
{
    ui->setupUi(this);

    connect(device, &QSerialPort::errorOccurred, this, &MainWindow::onErrorOccurred);
    connect(device, &QSerialPort::readyRead, this, &MainWindow::readFromPort);
    connect(timer, &QTimer::timeout, this, &MainWindow::onTimerInterrupt);
    connect(plotTimer, &QTimer::timeout, this, &MainWindow::onPlotTimerTick);

    plotTimer->start(20);
    ui->wykres1->addGraph();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonSzukaj_clicked()
{

    qDebug() <<"Wcisnieto przycisk Szukaj";
    QList<QSerialPortInfo> devices;
    devices  = QSerialPortInfo::availablePorts();
    for (int i = 0; i < devices.length(); i++){
        qDebug() << devices.at(i).portName() << devices.at(i).description();
        ui->comboBox->addItem(devices.at(i).portName()+ "\t" + devices.at(i).description());
        this->addToLogs("Znaleziono urzadzenie " + devices.at(i).portName()+ "\t" + devices.at(i).description());
    }
}

void MainWindow::addToLogs(QString message)
{
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
    ui->textEdit->append(currentDateTime + "\t" + message);
}

void MainWindow::on_pushButtonPolacz_clicked()
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
        this->timer->start(1000);
    } else {
        this->addToLogs("Otwarcie portu szeregowego się nie powiodło!");
    }
}

void MainWindow::on_pushButtonRozlacz_clicked()
{
    if (device->isOpen()) {
        device->close();
        addToLogs("Zamknięto połączenie.");
    }
    timer->stop();
    run = false;
}

void MainWindow::onErrorOccurred(QSerialPort::SerialPortError error)
{
    qDebug() << error << device->error();
}


void MainWindow::readFromPort() {
    while(this->device->canReadLine()) {
        QString line = QString::fromLocal8Bit(this->device->readLine()).trimmed();
        QString terminator = "\n";
        int pos = line.lastIndexOf(terminator);
        this->addToLogs(line.left(pos));
        if(line.startsWith("P=")) {
            QString params = line.mid(2);
            QStringList list = params.split(",");
            if(list.length() == 5) {
                freq  = list[0].toDouble();
                amp   = list[1].toDouble();
                bias  = list[2].toDouble();
                shape = list[3].toInt();
                int status = list[4].toInt();
                run = (status == 1);
            }
        }
    }
}

void MainWindow::onTimerInterrupt(){
    double val = 0;
    static int zegar = 0;
    static double czas = 0;
    static QList<double> time;
    static QList<double> value;
    zegar++;
    czas=czas+0.1;
    time.append(czas);
    value.append(val);
    AC1Wyjscie = ui->dial->value();
    AC2Wyjscie = ui->dial_2->value();
    AC3Wyjscie = ui->dial_3->value();
    QString str = "IN=";
    str=str + QString::number(StanWejsc)+","+ QString::number(AC1Wyjscie)+","+ QString::number(AC2Wyjscie)+","+ QString::number(AC3Wyjscie)+"\n";
    this->device->write(str.toLocal8Bit());
}

void MainWindow::onPlotTimerTick()
{
    if(!run) {
        return;
    }
    double t = plotTime;
    double val = 0.0;

    if (shape == 0) { //sinus
        val = bias + amp * sin(3.1416 * 2 * freq * t);
    }
    else if (shape == 1) { //prostokat
        double sinVal = sin(3.1416 * 2 * freq * t);
        val = (sinVal >= 0) ? (bias + amp) : (bias - amp);
    }
    else if (shape == 2) { //pila
        double cycle = 0.0;
        if (freq != 0.0) cycle = std::fmod(t * freq, 1.0);
        val = bias + (cycle * 2.0 * amp) - amp;
    }
    ui->wykres1->graph(0)->addData(t, val);
    ui->wykres1->xAxis->setRange(t, 5, Qt::AlignCenter);
    ui->wykres1->yAxis->rescale(true);
    double margin = amp * 0.1;
    if (margin == 0) margin = 1.0;

    double bottomLimit = (bias - amp) - margin;
    double topLimit    = (bias + amp) + margin;
    ui->wykres1->yAxis->setRange(bottomLimit, topLimit);
    ui->wykres1->replot();
    plotTime += 0.01;
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{

    if(arg1 == 2){
        StanWejsc=StanWejsc+1;
    }
    else StanWejsc=StanWejsc-1;;
    qDebug()<<StanWejsc;

    ui->StanWejsc->setText(QString::number(StanWejsc));
}

void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    qDebug()<< arg1;
    if(arg1 == 2){
        StanWejsc=StanWejsc+2;
    }
    else StanWejsc=StanWejsc-2;

    ui->StanWejsc->setText(QString::number(StanWejsc));

}

void MainWindow::on_checkBox_3_stateChanged(int arg1)
{
    if(arg1 == 2){
        StanWejsc=StanWejsc+4;
    }
    else StanWejsc=StanWejsc-4;

    ui->StanWejsc->setText(QString::number(StanWejsc));

}

void MainWindow::on_checkBox_4_stateChanged(int arg1)
{    if(arg1 == 2){
        StanWejsc=StanWejsc+8;
    }
    else StanWejsc=StanWejsc-8;

    ui->StanWejsc->setText(QString::number(StanWejsc));

}

void MainWindow::on_checkBox_8_stateChanged(int arg1)
{
    if(arg1 == 2){
        StanWejsc=StanWejsc+16;
    }
    else StanWejsc=StanWejsc-16;

    ui->StanWejsc->setText(QString::number(StanWejsc));

}

void MainWindow::on_checkBox_5_stateChanged(int arg1)
{
    if(arg1 == 2){
        StanWejsc=StanWejsc+32;
    }
    else StanWejsc=StanWejsc-32;

    ui->StanWejsc->setText(QString::number(StanWejsc));
}

void MainWindow::on_checkBox_6_stateChanged(int arg1)
{
    if(arg1 == 2){
        StanWejsc=StanWejsc+64;
    }
    else StanWejsc=StanWejsc-64;

    ui->StanWejsc->setText(QString::number(StanWejsc));

}

void MainWindow::on_checkBox_7_stateChanged(int arg1)
{
    if(arg1 == 2){
        StanWejsc=StanWejsc+128;
    }
    else StanWejsc=StanWejsc-128;

    ui->StanWejsc->setText(QString::number(StanWejsc));

}
