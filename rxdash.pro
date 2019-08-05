QT += quick serialport
CONFIG += c++11

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CFLAGS += -Wall -Wextra -Wno-unused-result -Wno-unused-function
QMAKE_CXXFLAGS += -Wall -Wextra -Wno-unused-result -Wno-unused-function

SOURCES += main.cpp \
    SyncSerialPort.cpp \
    DashDriver.cpp \
    PfcBus.cpp \
    utils.cpp \
    Pfc.cpp

HEADERS += \
    SyncSerialPort.h \
    DashDriver.h \
    Pfc.h \
    PfcBus.h \
    PfcRaw.h \
    utils.h

SOURCES += deps/wiringPi/wiringPi.c \
    deps/wiringPi/wiringSerial.c \
    deps/wiringPi/softPwm.c \
    deps/wiringPi/softTone.c \
    deps/wiringPi/piHiPri.c

DISTFILES +=

RESOURCES += qml.qrc
