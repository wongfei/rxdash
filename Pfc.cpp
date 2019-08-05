#include "Pfc.h"

PfcException::PfcException() : mLocation("") {}

PfcException::PfcException(const char* location, const QString& msg)
    : mLocation(location), mMessage(msg) {}

PfcException* PfcException::clone() const {
    return new PfcException(mLocation, mMessage);
}

void PfcException::raise() const {
    throw *this;
}

const QString PfcException::toString() const {
    return QString("%1 (%2)").arg(mMessage, QString(mLocation));
}
