#pragma once

#include "Pfc.h"

class PfcBus : public QObject {
    Q_OBJECT

public:
    explicit PfcBus(QObject *parent = nullptr);
    virtual ~PfcBus();

    void openPort(const QString& portName, int pollTimeout, int readWriteTimeout);
    void closePort();
    void resetData();
    bool isOpen() const;

    void readVer();
    void readBasic();
    void readAdvanced();
    void readSensors();
    void readAux();

    void sendBytes(const QByteArray& data);
    void sendHex(const char* data);
    void recvPacket(int id, int size);

    void setVar(Pfc::Id id, float value);
    float getVar(Pfc::Id id) const;

    QString getVer1() const { return mVer1; }
    QString getVer2() const { return mVer2; }
    QString getModel() const { return mModel; }
    QString getNames() const { return mNames; }

    const PfcRawBasic& getRawBasic() const { return mRawBasic; }
    const PfcRawAdvanced& getRawAdvanced() const { return mRawAdvanced; }
    const PfcRawSensors& getRawSensors() const { return mRawSensors; }
    const PfcRawAux& getRawAux() const { return mRawAux; }

protected:
    QScopedPointer<class QSyncSerialPort> mSerial;
    QByteArray mResponse;
    QString mVer1;
    QString mVer2;
    QString mModel;
    QString mNames;
    PfcRawBasic mRawBasic;
    PfcRawAdvanced mRawAdvanced;
    PfcRawSensors mRawSensors;
    PfcRawAux mRawAux;
    float mVars[Pfc::Id_Count];
};
