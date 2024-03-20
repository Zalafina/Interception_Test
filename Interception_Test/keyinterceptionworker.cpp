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

QString GetDeviceNameByHardwareID(const QString& hardwareID)
{
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(nullptr, nullptr, nullptr, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (deviceInfoSet == INVALID_HANDLE_VALUE)
    {
        // 处理错误
        return QString();
    }

    SP_DEVINFO_DATA deviceInfoData;
    ZeroMemory(&deviceInfoData, sizeof(SP_DEVINFO_DATA));
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    DWORD index = 0;
    while (SetupDiEnumDeviceInfo(deviceInfoSet, index, &deviceInfoData))
    {
        DWORD dataType;
        WCHAR buffer[MAX_PATH];
        DWORD bufferSize = sizeof(buffer);

        if (SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_HARDWAREID,
                                             &dataType, reinterpret_cast<PBYTE>(buffer), bufferSize, nullptr))
        {
            QString hardwareIdStr = QString::fromWCharArray(buffer);
            if (hardwareIdStr == hardwareID)
            {
                if (SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_DEVICEDESC,
                                                     &dataType, reinterpret_cast<PBYTE>(buffer), bufferSize, nullptr))
                {
                    QString deviceName = QString::fromWCharArray(buffer);
                    SetupDiDestroyDeviceInfoList(deviceInfoSet);
                    return deviceName;
                }
            }
        }

        index++;
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return QString();
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

    wchar_t hardware_id[500];
    QList<InterceptionDevice> keyboard_devicelist;
    QList<InterceptionDevice> mouse_devicelist;
    for(InterceptionDevice device = INTERCEPTION_KEYBOARD(0); device <= INTERCEPTION_MAX_DEVICE; ++device)
    {
        if (interception_is_keyboard(device)) {
            size_t length = interception_get_hardware_id(context, device, hardware_id, sizeof(hardware_id));

            if(length > 0 && length < sizeof(hardware_id)) {
                QString hardware_id_str = QString::fromWCharArray(hardware_id);
                QString devicename = GetDeviceNameByHardwareID(hardware_id_str);
                qDebug().nospace() << "[KeyInterceptionWorker] Valid Keyboard[" << device << "] -> HardwareID: " << hardware_id_str << ", DeviceName: " << devicename;

                keyboard_devicelist.append(device);
            }
            else {
                qDebug().nospace() << "[KeyInterceptionWorker] Invalid Keyboard[" << device << "]";
            }
        }
        else if (interception_is_mouse(device)) {
            size_t length = interception_get_hardware_id(context, device, hardware_id, sizeof(hardware_id));

            if(length > 0 && length < sizeof(hardware_id)) {
                QString hardware_id_str = QString::fromWCharArray(hardware_id);
                QString devicename = GetDeviceNameByHardwareID(hardware_id_str);
                qDebug().nospace() << "[KeyInterceptionWorker] Valid Mouse[" << device << "] -> HardwareID: " << hardware_id_str << ", DeviceName: " << devicename;
                mouse_devicelist.append(device);
            }
            else {
                qDebug().nospace() << "[KeyInterceptionWorker] Invalid Mouse[" << device << "]";
            }
        }
    }

    interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);
    interception_set_filter(context, interception_is_mouse, INTERCEPTION_FILTER_MOUSE_ALL);

    while(!exist_flag && interception_receive(context, device = interception_wait(context), (InterceptionStroke *)&stroke, 1) > 0)
    {
        if(interception_is_keyboard(device))
        {
            unsigned int keyboard_index = 1;
            InterceptionKeyStroke &kstroke = *(InterceptionKeyStroke *) &stroke;
            unsigned int ori_information = kstroke.information;
            keyboard_index += device - INTERCEPTION_KEYBOARD(0);
            kstroke.information = 0xAAAA0000 + keyboard_index;
            QString ori_extraInfoStr = QString("0x%1").arg(QString::number(ori_information, 16).toUpper(), 8, '0');
            QString extraInfoStr = QString("0x%1").arg(QString::number(kstroke.information, 16).toUpper(), 8, '0');
            qDebug().nospace() << "[KeyInterceptionWorker] Keyboard[" << device << "] extraInfo (" << ori_extraInfoStr << " -> " << extraInfoStr << ")";
            interception_send(context, device, (InterceptionStroke *)&stroke, 1);
        }

        if(interception_is_mouse(device))
        {
            unsigned int mouse_index = 1;
            InterceptionMouseStroke &mstroke = *(InterceptionMouseStroke *) &stroke;
            // unsigned int ori_information = mstroke.information;
            mouse_index += device - INTERCEPTION_MOUSE(0);
            mstroke.information = 0xBBBB0000 + mouse_index;
            // if (mstroke.state != 0) {
            //     QString ori_extraInfoStr = QString("0x%1").arg(QString::number(ori_information, 16).toUpper(), 8, '0');
            //     QString extraInfoStr = QString("0x%1").arg(QString::number(mstroke.information, 16).toUpper(), 8, '0');
            //     qDebug().nospace() << "[KeyInterceptionWorker] Mouse[" << device << "] extraInfo (" << ori_extraInfoStr << " -> " << extraInfoStr << ")";
            // }
            interception_send(context, device, (InterceptionStroke *)&stroke, 1);
        }
    }

    interception_destroy_context(context);
}
