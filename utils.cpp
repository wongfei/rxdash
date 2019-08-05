#include "utils.h"
#include <QProcess>

namespace ut {

QString osGetEnv(const QString& name, const QString& defValue) {
    QProcessEnvironment env;
    return env.value(name, defValue);
}

void osPowerOff() {
    system("/bin/sync");
    system("/usr/bin/sudo /sbin/shutdown -h now");
}

} // namespace
