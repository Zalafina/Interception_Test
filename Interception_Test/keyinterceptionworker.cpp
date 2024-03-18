#include "keyinterceptionworker.h"

KeyInterceptionWorker::KeyInterceptionWorker(QObject *parent)
    : QObject{parent}
{}

void KeyInterceptionWorker::doWork()
{
    InterceptionContext context;
    InterceptionDevice device;
    InterceptionKeyStroke stroke;

    raise_process_priority();

    context = interception_create_context();

    interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);
    interception_set_filter(context, interception_is_mouse, INTERCEPTION_FILTER_MOUSE_ALL);

    while(interception_receive(context, device = interception_wait(context), (InterceptionStroke *)&stroke, 1) > 0)
    {
        if(interception_is_keyboard(device))
        {
            InterceptionKeyStroke &kstroke = *(InterceptionKeyStroke *) &stroke;
            // unsigned int ori_information = kstroke.information;
            kstroke.information = 0xAAAA0001;
            // QString ori_extraInfoStr = QString("0x%1").arg(QString::number(ori_information, 16).toUpper(), 8, '0');
            // QString extraInfoStr = QString("0x%1").arg(QString::number(kstroke.information, 16).toUpper(), 8, '0');
            // qDebug().nospace() << "[KeyInterceptionWorker] Keyboard change extraInfo [" << ori_extraInfoStr << " -> " << extraInfoStr << "]";
            interception_send(context, device, (InterceptionStroke *)&stroke, 1);
        }

        if(interception_is_mouse(device))
        {
            InterceptionMouseStroke &mstroke = *(InterceptionMouseStroke *) &stroke;
            // unsigned int ori_information = mstroke.information;
            mstroke.information = 0xBBBB0001;
            // QString ori_extraInfoStr = QString("0x%1").arg(QString::number(ori_information, 16).toUpper(), 8, '0');
            // QString extraInfoStr = QString("0x%1").arg(QString::number(mstroke.information, 16).toUpper(), 8, '0');
            // qDebug().nospace() << "[KeyInterceptionWorker] Keyboard change extraInfo [" << ori_extraInfoStr << " -> " << extraInfoStr << "]";
            interception_send(context, device, (InterceptionStroke *)&stroke, 1);
        }

        // if (stroke.code == SCANCODE_X) {
        //     stroke.code = SCANCODE_Y;
        //     stroke.information = 0xFFFF0001;
        // }

        // interception_send(context, device, (InterceptionStroke *)&stroke, 1);
    }

    interception_destroy_context(context);
}
