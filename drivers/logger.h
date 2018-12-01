#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

class Logger : public QObject
{
    Q_OBJECT

public:
    explicit Logger(QObject *parent = nullptr);

signals:
    void loggersignal();

public slots:
    bool sendString(QString message);

private:
    bool openDevice();
    bool closeDevice();

    int fd;

    const static QString UART_DEVICE_STRING;

};


#endif // LOGGER_H
