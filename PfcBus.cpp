#include "PfcBus.h"
#include "SyncSerialPort.h"
#include "utils.h"

//
// https://www.rx7club.com/power-fc-forum-47/hacking-powerfc-datalogit-147597/
//

PfcBus::PfcBus(QObject* parent) : QObject(parent) {
    resetData();
}

PfcBus::~PfcBus() {
    closePort();
}

void PfcBus::openPort(const QString& portName, int pollTimeout, int readWriteTimeout) {
    qInfo() << "PfcBus: openPort:" << portName;

    mSerial.reset(new QSyncSerialPort());
    mSerial->setPortName(portName);
    mSerial->setBaudRate(QSerialPort::Baud57600);
    mSerial->setDataBits(QSerialPort::Data8);
    mSerial->setParity(QSerialPort::NoParity);
    mSerial->setStopBits(QSerialPort::OneStop);
    mSerial->setFlowControl(QSerialPort::NoFlowControl);
    mSerial->setPollTimeout(pollTimeout);
    mSerial->setReadTimeout(readWriteTimeout);
    mSerial->setWriteTimeout(readWriteTimeout);

    if (!mSerial->open(QIODevice::ReadWrite)) {
        throw PfcException(CODE_LOCATION, "open failed");
    }
}

void PfcBus::closePort() {
    if (!mSerial.isNull()) {
        if (mSerial->isOpen()) {
            qInfo() << "PfcBus: closePort:" << mSerial->portName();
        }
        mSerial.reset();
    }
    resetData();
}

void PfcBus::resetData() {
    mResponse.clear();
    mVer1 = "";
    mVer2 = "";
    mModel = "";
    mNames = "";
    memset(&mRawBasic, 0, sizeof(mRawBasic));
    memset(&mRawAdvanced, 0, sizeof(mRawAdvanced));
    memset(&mRawSensors, 0, sizeof(mRawSensors));
    memset(&mRawAux, 0, sizeof(mRawAux));
    memset(&mVars[0], 0, sizeof(mVars));
}

bool PfcBus::isOpen() const {
    return (!mSerial.isNull() && mSerial->isOpen());
}

void PfcBus::readVer() {
    sendHex("0102FC");
    recvPacket(0x01, 8);
    mVer1 = QString(mResponse).mid(2, 4);
    qInfo() << "VER1" << mVer1;

    sendHex("F3020A");
    recvPacket(0xF3, 11);
    mModel = QString(mResponse).mid(2, 8);
    qInfo() << "MODEL" << mModel;

    sendHex("F50208");
    recvPacket(0xF5, 8);
    mVer2 = QString(mResponse).mid(2, 4);
    qInfo() << "VER2" << mVer2;

    sendHex("DD0220");
    recvPacket(0xDD, 83);
    mNames = QString(mResponse).mid(2, 80);
    qInfo() << "NAMES" << mNames;
}

void PfcBus::readBasic() {
    sendHex("DA0223");
    recvPacket(0xDA, 23);
    auto raw = reinterpret_cast<const PfcRawBasic*>(mResponse.data());
    mRawBasic = *raw;

    setVar(Pfc::B_InjDuty, raw->InjDuty * 0.1f);
    setVar(Pfc::B_IGL, raw->IGL - 25);
    setVar(Pfc::B_IGT, raw->IGT - 25);
    setVar(Pfc::B_RPM, raw->RPM);
    setVar(Pfc::B_Speed, raw->Speed);
    setVar(Pfc::B_Boost, raw->Boost); // TODO: ?
    setVar(Pfc::B_Knock, raw->Knock);
    setVar(Pfc::B_WtrT, raw->WtrT - 80);
    setVar(Pfc::B_AirT, raw->AirT - 80);
    setVar(Pfc::B_BatV, raw->BatV * 0.1f);
}

void PfcBus::readAdvanced() {
    sendHex("F0020D");
    recvPacket(0xF0, 33);
    auto raw = reinterpret_cast<const PfcRawAdvanced*>(mResponse.data());
    mRawAdvanced = *raw;

    setVar(Pfc::Adv_RPM, raw->RPM);
    setVar(Pfc::Adv_PIM, raw->PIM);
    setVar(Pfc::Adv_PIMV, raw->PIMV / 1000.0f);
    setVar(Pfc::Adv_TPSV, raw->TPSV / 1000.0f);
    setVar(Pfc::Adv_InjFrPr, raw->InjFrPr * (4 / 1000.0f)); // TODO: ?
    setVar(Pfc::Adv_InjPm, raw->InjPm);
    setVar(Pfc::Adv_IGL, raw->IGL - 25);
    setVar(Pfc::Adv_IGT, raw->IGT - 25);
    setVar(Pfc::Adv_FuelT, raw->FuelT - 80);
    setVar(Pfc::Adv_Oil, raw->Oil);
    setVar(Pfc::Adv_PC, raw->PC);
    setVar(Pfc::Adv_WG, raw->WG);
    setVar(Pfc::Adv_WtrT, raw->WtrT - 80);
    setVar(Pfc::Adv_AirT, raw->AirT - 80);
    setVar(Pfc::Adv_Knock, raw->Knock);
    setVar(Pfc::Adv_BatV, raw->BatV / 10.0f);
    setVar(Pfc::Adv_Speed, raw->Speed);
    setVar(Pfc::Adv_Unk1, raw->Unk1); // TODO: ?
    setVar(Pfc::Adv_O2S, raw->O2S);
    setVar(Pfc::Adv_Unk2, raw->Unk2); // TODO: ?
    setVar(Pfc::Adv_InjFrSc, raw->InjFrSc * (4 / 1000.0f)); // TODO: ?
}

void PfcBus::readSensors() {
    sendHex("DE021F");
    recvPacket(0xDE, 21);
    auto raw = reinterpret_cast<const PfcRawSensors*>(mResponse.data());
    mRawSensors = *raw;

    setVar(Pfc::Sens_PIM, raw->PIM * 0.01f);
    setVar(Pfc::Sens_VTA1, raw->VTA1 * 0.01f);
    setVar(Pfc::Sens_VTA2, raw->VTA2 * 0.01f);
    setVar(Pfc::Sens_VMOP, raw->VMOP * 0.01f);
    setVar(Pfc::Sens_WTRT, raw->WTRT * 0.01f);
    setVar(Pfc::Sens_AIRT, raw->AIRT * 0.01f);
    setVar(Pfc::Sens_FUEL, raw->FUEL * 0.01f);
    setVar(Pfc::Sens_O2S, raw->O2S * 0.01f);

    auto flags = raw->Flags;
    setVar(Pfc::Sw_STR, ut::getbit(flags, 0));
    setVar(Pfc::Sw_AC, ut::getbit(flags, 1));
    setVar(Pfc::Sw_PWS, ut::getbit(flags, 2));
    setVar(Pfc::Sw_NTR, ut::getbit(flags, 3));
    setVar(Pfc::Sw_CLT, ut::getbit(flags, 4));
    setVar(Pfc::Sw_STP, ut::getbit(flags, 5));
    setVar(Pfc::Sw_CAT, ut::getbit(flags, 6));
    setVar(Pfc::Sw_ELD, ut::getbit(flags, 7));
    setVar(Pfc::Sw_HWL, ut::getbit(flags, 8));
    setVar(Pfc::Sw_FPD, ut::getbit(flags, 9));
    setVar(Pfc::Sw_FPR, ut::getbit(flags, 10));
    setVar(Pfc::Sw_APR, ut::getbit(flags, 11));
    setVar(Pfc::Sw_PAC, ut::getbit(flags, 12));
    setVar(Pfc::Sw_CCN, ut::getbit(flags, 13));
    setVar(Pfc::Sw_TCN, ut::getbit(flags, 14));
    setVar(Pfc::Sw_PRC, ut::getbit(flags, 15));
}

inline float aux2v(uint16_t aux) { return ((aux / 1023.0f) * 5.0f); };

void PfcBus::readAux() {
    sendHex("010300FB");
    recvPacket(0x01, 19);
    auto raw = reinterpret_cast<const PfcRawAux*>(mResponse.data());
    mRawAux = *raw;

    setVar(Pfc::Aux1, aux2v(raw->Data1));
    setVar(Pfc::Aux2, aux2v(raw->Data2));
    setVar(Pfc::Aux3, aux2v(raw->Data3));
    setVar(Pfc::Aux4, aux2v(raw->Data4));
    setVar(Pfc::Aux5, aux2v(raw->Data5));
    setVar(Pfc::Aux6, aux2v(raw->Data6));
    setVar(Pfc::Aux7, aux2v(raw->Data7));
    setVar(Pfc::Aux8, aux2v(raw->Data8));
}

void PfcBus::sendBytes(const QByteArray& data) {
    if (data.size() <= 0) {
        throw PfcException(CODE_LOCATION, QString("invalid data"));
    }
    if (!mSerial->writeSync(data)) {
        throw PfcException(CODE_LOCATION, QString("sendBytes failed: id=%1").arg(static_cast<int>(data[0])));
    }
}

void PfcBus::sendHex(const char* data) {
    if (!data) {
        throw PfcException(CODE_LOCATION, QString("invalid data"));
    }
    sendBytes(QByteArray::fromHex(data));
}

void PfcBus::recvPacket(int id, int size) {
    if (size <= 0) {
        throw PfcException(CODE_LOCATION, QString("invalid size"));
    }
    if (!mSerial->readSync(mResponse, size)) {
        throw PfcException(CODE_LOCATION, QString("recvPacket failed: id=%1").arg(id));
    }
    const int respSize = mResponse.size();
    const int respId = (respSize > 0 ? mResponse[0] : 0);
    if (respSize <= 0 || respSize != size || respId != id) {
        QString msg;
        msg.sprintf("invalid packet: id=%d size=%d (expected id=%d size=%d)", respId, respSize, id, size);
        throw PfcException(CODE_LOCATION, msg);
    }
}

void PfcBus::setVar(Pfc::Id id, float value) {
    if (Pfc::isValid(id)) {
        mVars[id] = value;
    }
}

float PfcBus::getVar(Pfc::Id id) const {
    return (Pfc::isValid(id) ? mVars[id] : 0);
}
