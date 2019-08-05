#include "SyncSerialPort.h"

bool QSyncSerialPort::readSync(QByteArray& response, int size) {
    response.clear();
    if (size <= 0) {
        return false;
    }
    char buf[1024];
    QElapsedTimer timer;
    timer.start();
    for (;;) {
        if ((bytesAvailable() > 0) || waitForReadyRead(mPollTimeout)) {
            const qint64 chunkSize = qMin<qint64>(size - response.size(), sizeof(buf));
            const qint64 n = read(buf, chunkSize);
            if (n <= 0) {
                return false;
            }
            response.append(buf, static_cast<int>(n));
            if (response.size() >= size) {
                break;
            }
        }
        if (timer.hasExpired(mReadTimeout)) {
            return false;
        }
    }
    return true;
}

bool QSyncSerialPort::writeSync(const char* data, int size) {
    if (!data || size <= 0) {
        return false;
    }
    const qint64 n = write(data, size);
    if (n <= 0) {
        return false;
    }
    QElapsedTimer timer;
    timer.start();
    for (;;) {
        waitForBytesWritten(mPollTimeout);
        if (bytesToWrite() <= 0) {
            break;
        }
        if (timer.hasExpired(mWriteTimeout)) {
            return false;
        }
    }
    return true;
}

bool QSyncSerialPort::writeSync(const QByteArray& data) {
    return writeSync(data.constData(), data.size());
}

void QSyncSerialPort::setPollTimeout(int value) {
    mPollTimeout = value;
}

void QSyncSerialPort::setReadTimeout(int value) {
    mReadTimeout = value;
}

void QSyncSerialPort::setWriteTimeout(int value) {
    mWriteTimeout = value;
}
