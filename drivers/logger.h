#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <termios.h>

class Logger : public QObject
{
    Q_OBJECT

public:
    explicit Logger(QObject *parent = nullptr);
    explicit Logger(QObject *parent, QString SerialDevicePath, speed_t Baudrate);
    ~Logger();

    bool openSerialConnection(QString DevicePath, speed_t BaudRate);
    bool closeSerialConnection();
    bool isConnectionOpen();

signals:
    void loggersignal();

public slots:
    bool sendString(QString message);

private:

    int fd;
    bool isFileOpen;

    const static QString DEFAULT_UART_DEVICE_STRING;
    const static speed_t DEFAULT_BAUDRATE;

};


#endif // LOGGER_H
