#include "qserailthread.h"
#include "QDebug"
#include "dataproduct.h"

QserailThread::QserailThread(QObject *parent) :
    QThread(parent)
{
    ff = fifo_init(32768);
}

QserailThread::~QserailThread()
{
    this->CloseSerialPort();
}

void QserailThread::OpenSerialPort(QString port)
{
    m_portName = port;
    if (m_portName.isEmpty()) {
        qDebug() << tr("No port name specified");
        return;
    }
    serial.setPortName(m_portName);
    serial.setBaudRate(921600);
    serial.setReadBufferSize(2000);
    //    serial.setStopBits();
    //    serial.setDataBits();
    if (!serial.open(QIODevice::ReadOnly)) {
        qDebug() <<tr("Can't open %1, error code %2")
                        .arg(m_portName).arg(serial.error());
        return;
    }
    else
    {
        m_state = true;
        this->serial.moveToThread(this);
        this->start();
    }
}

void QserailThread::CloseSerialPort()
{
    serial.close();
    m_state = false;
    this->quit();
}

void QserailThread::run()
{
    char head;
    QByteArray qb;
    serial.clear();
    while (m_quit) {
        if(m_state) {
            while(serial.waitForReadyRead(10))
            {
                qb = serial.readAll();
                fifo_put_force(ff,qb.data(),qb.size());
            }
        }
        msleep(10);
    }
}

