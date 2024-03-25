#ifndef KEYINTERCEPTIONWORKER_H
#define KEYINTERCEPTIONWORKER_H

#include <QObject>
#include <QDebug>
#include <interception.h>
#include <windows.h>
#include <setupapi.h>
#include <libusb.h>
// #include <utils.h>

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
    ~KeyInterceptionWorker();

    void displayUSBDeviceStrings(libusb_device_handle *dev_handle);

signals:

public slots:
    void doWork(void);

private:
    bool exist_flag;
};

#endif // KEYINTERCEPTIONWORKER_H
