#include "DashDriver.h"
#include <QCoreApplication>
#include "utils.h"

#if defined(PLATFORM_RPI)
    #include "deps/wiringPi/wiringPi.h"
    #define RPI_POWEROFF_PIN 12 // physical id
#endif

void DashDriver::staticInit() {
    #if defined(PLATFORM_RPI)
    wiringPiSetupPhys();
    pinMode(RPI_POWEROFF_PIN, INPUT);
    #endif
}

DashDriver::DashDriver(QObject* parent) : QThread(parent) {
    qInfo() << "DashDriver: start";

    mPfc.reset(new PfcBus());
    mPfcTimeout.invalidate();
    mPfcDebugTimer.start();

    mRunFlag = true;
    start(QThread::NormalPriority);
}

DashDriver::~DashDriver() {
    if (isRunning()) {
        mRunFlag = false;
        qInfo() << "DashDriver: stop";
        wait();
    }
}

void DashDriver::run() {
    qInfo() << "DashDriver: enter worker thread";
    try {

        mainLoop();

    } catch (const PfcException& ex) {
        qCritical() << "DashDriver: PfcException:" << ex.toString();

    } catch (const std::exception& ex) {
        qCritical() << "DashDriver: exception:" << ex.what();

    } catch (...) {
        qCritical() << "DashDriver: unknown exception";
    }
    qInfo() << "DashDriver: exit worker thread";
}

void DashDriver::mainLoop() {
    qInfo() << "DashDriver: mainLoop";

    while (mRunFlag) {
        #if defined(PLATFORM_RPI)
        if (digitalRead(RPI_POWEROFF_PIN) == 1) {
            qInfo() << "DashDriver: POWEROFF";
            QCoreApplication::exit(RC_POWEROFF);
            return;
        }
        #endif

        updatePfc();

        if (mPfc->isOpen()) {
            msleep(0);
        }
        else {
            msleep(100); // just idle
        }
    }
}

void DashDriver::updatePfc() {
    try {
        if (mPfc->isOpen()) {
            mPfc->readAdvanced();
            mPfc->readSensors();
            mPfc->readAux();
            mPfcSamples++;
        }
        else {
            if (!mPfcTimeout.isValid() || (mPfcTimeout.isValid() && mPfcTimeout.hasExpired(mReconnectTimeout))) {
                mPfcTimeout.start();
                mPfcSamples = 0;
                mPfcRate = 0;

                mPfc->openPort(mPortName, mPollTimeout, mReadWriteTimeout);
                mPfc->readVer();
            }
        }

        if (mPfcDebugTimer.hasExpired(1000)) {
            mPfcDebugTimer.start();
            mPfcRate = mPfcSamples;
            mPfcSamples = 0;
        }

    } catch (const PfcException& ex) {
        qCritical() << "DashDriver: PfcException:" << ex.toString();
        mPfc->closePort();
        mPfcTimeout.start();
        ++mPfcErrors;
    }
}

QString DashDriver::getSwitchesStr() const {
    QString str;
    str.reserve(16);
    const quint16 flags = mPfc->getRawSensors().Flags;
    for (int i = 0; i < 16; ++i) {
        auto value = ut::getbit(flags, i);
        str.append(value ? '1' : '0');
    }
    return str;
}

bool DashDriver::isConnected() const { return mPfc->isOpen(); }
float DashDriver::getVar(Pfc::Id id) const { return mPfc->getVar(id); }
unsigned int DashDriver::getRenderRate() const { return static_cast<unsigned int>(mRenderRate); }
unsigned int DashDriver::getBusErrorCount() const { return mPfcErrors; }
unsigned int DashDriver::getBusSampleCount() const { return mPfcSamples; }
unsigned int DashDriver::getBusUpdateRate() const { return mPfcRate; }
