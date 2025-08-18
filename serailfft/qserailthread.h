#ifndef QSERAILTHREAD_H
#define QSERAILTHREAD_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <QSerialPort>
#include "fifo.h"
#include "dataproduct.h"

class QserailThread: public QThread
{
  Q_OBJECT
public:
   explicit QserailThread(QObject *parent = nullptr);
   ~QserailThread();
   void OpenSerialPort(QString port);
   void CloseSerialPort();
   int responseData = 0;
   struct fifo* ff;

private:
   void run() override;
   QSerialPort serial;
   QString m_portName;
   bool m_state = false;
   bool m_quit = true;
};

#endif // QSERAILTHREAD_H
