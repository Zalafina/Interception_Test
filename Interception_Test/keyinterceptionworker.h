#ifndef KEYINTERCEPTIONWORKER_H
#define KEYINTERCEPTIONWORKER_H

#include <QObject>
#include <QDebug>
#include <interception.h>
#include <utils.h>

enum ScanCode
{
    SCANCODE_X   = 0x2D,
    SCANCODE_Y   = 0x15,
    SCANCODE_ESC = 0x01
};

class KeyInterceptionWorker : public QObject
{
    Q_OBJECT
public:
    explicit KeyInterceptionWorker(QObject *parent = nullptr);

signals:

public slots:
    void doWork(void);
};

#endif // KEYINTERCEPTIONWORKER_H
