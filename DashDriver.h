#pragma once

#include "PfcBus.h"
#include <QThread>
#include <QElapsedTimer>

class DashDriver : public QThread {
    Q_OBJECT

public:
    enum { RC_POWEROFF = 9000 };

    static void staticInit();

    explicit DashDriver(QObject *parent = nullptr);
    virtual ~DashDriver();

    Q_INVOKABLE bool isConnected() const;
    Q_INVOKABLE float getVar(Pfc::Id id) const;
    Q_INVOKABLE unsigned int getRenderRate() const;
    Q_INVOKABLE unsigned int getBusErrorCount() const;
    Q_INVOKABLE unsigned int getBusSampleCount() const;
    Q_INVOKABLE unsigned int getBusUpdateRate() const;
    Q_INVOKABLE QString getSwitchesStr() const;

protected:
    void run() override;
    void mainLoop();
    void updatePfc();

protected:
    volatile int mRunFlag = 0;

    // config
    QString mPortName = "/dev/ttyUSB0";
    int mPollTimeout = 10;
    int mReadWriteTimeout = 200;
    int mReconnectTimeout = 10000;
    int mRenderRate = 1000 / 25;

    QScopedPointer<class PfcBus> mPfc;
    QElapsedTimer mPfcTimeout;
    QElapsedTimer mPfcDebugTimer;
    unsigned int mPfcErrors = 0;
    unsigned int mPfcSamples = 0;
    unsigned int mPfcRate = 0;
};
