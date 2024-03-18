#include "interceptiontest.h"
#include "keyinterceptionworker.h"

#include <QApplication>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    KeyInterceptionWorker *worker = new KeyInterceptionWorker;
    QThread *workerThread = new QThread;

    worker->moveToThread(workerThread);
    QObject::connect(workerThread, &QThread::started, worker, &KeyInterceptionWorker::doWork);
    workerThread->start();

    InterceptionTest w;
    w.show();
    return a.exec();
}
