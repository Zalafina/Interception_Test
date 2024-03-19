#include "keyinterceptionworker.h"

KeyInterceptionWorker::KeyInterceptionWorker(QObject *parent)
    : QObject{parent},
    exist_flag(false)
{}

QString GetSystem32DirectoryPath()
{
    wchar_t buffer[MAX_PATH];
    UINT result = GetSystemDirectoryW(buffer, MAX_PATH);
    if (result == 0)
    {
        // 获取系统目录路径失败
        return QString();
    }

    return QString::fromWCharArray(buffer);
}

void KeyInterceptionWorker::doWork()
{
    InterceptionContext context;
    InterceptionDevice device;
    InterceptionKeyStroke stroke;

    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    context = interception_create_context();

    if (context == Q_NULLPTR) {
        qDebug().nospace() << "[KeyInterceptionWorker] interception_create_context Failed!!!";
    }
    else {
        qDebug().nospace() << "[KeyInterceptionWorker] interception_create_context Success.";
    }

    QString system32path = GetSystem32DirectoryPath();
    qDebug().nospace() << "[KeyInterceptionWorker] System32 Driver path :" << system32path;

    InterceptionDeviceArray device_array = (InterceptionDeviceArray)context;

    for(int i = INTERCEPTION_KEYBOARD(0); i <= INTERCEPTION_MAX_DEVICE; ++i)
    {

    }

    interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);
    interception_set_filter(context, interception_is_mouse, INTERCEPTION_FILTER_MOUSE_ALL);

    while(!exist_flag && interception_receive(context, device = interception_wait(context), (InterceptionStroke *)&stroke, 1) > 0)
    {
        if(interception_is_keyboard(device))
        {
            InterceptionKeyStroke &kstroke = *(InterceptionKeyStroke *) &stroke;
            unsigned int ori_information = kstroke.information;
            kstroke.information = 0xAAAA0001;
            QString ori_extraInfoStr = QString("0x%1").arg(QString::number(ori_information, 16).toUpper(), 8, '0');
            QString extraInfoStr = QString("0x%1").arg(QString::number(kstroke.information, 16).toUpper(), 8, '0');
            qDebug().nospace() << "[KeyInterceptionWorker] Keyboard change extraInfo [" << ori_extraInfoStr << " -> " << extraInfoStr << "]";
            interception_send(context, device, (InterceptionStroke *)&stroke, 1);
        }

        if(interception_is_mouse(device))
        {
            InterceptionMouseStroke &mstroke = *(InterceptionMouseStroke *) &stroke;
            // unsigned int ori_information = mstroke.information;
            mstroke.information = 0xBBBB0001;
            // if (mstroke.state != 0) {
            //     QString ori_extraInfoStr = QString("0x%1").arg(QString::number(ori_information, 16).toUpper(), 8, '0');
            //     QString extraInfoStr = QString("0x%1").arg(QString::number(mstroke.information, 16).toUpper(), 8, '0');
            //     qDebug().nospace() << "[KeyInterceptionWorker] Mouse change extraInfo [" << ori_extraInfoStr << " -> " << extraInfoStr << "]";
            // }
            interception_send(context, device, (InterceptionStroke *)&stroke, 1);
        }
    }

    interception_destroy_context(context);
}
