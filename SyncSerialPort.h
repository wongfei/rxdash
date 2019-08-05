#pragma once

#include <QtSerialPort/QtSerialPort>

class QSyncSerialPort : public QSerialPort {
    Q_OBJECT

public:
    bool readSync(QByteArray& response, int size);
    bool writeSync(const char* data, int size);
    bool writeSync(const QByteArray& data);

    void setPollTimeout(int value);
    void setReadTimeout(int value);
    void setWriteTimeout(int value);

protected:
    int mPollTimeout = 10;
    int mReadTimeout = 100;
    int mWriteTimeout = 100;
};
