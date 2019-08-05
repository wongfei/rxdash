#pragma once

#include "PfcRaw.h"
#include <QObject>
#include <QException>

#define STRINGIZE1(x) #x
#define STRINGIZE2(x) STRINGIZE1(x)
#define CODE_LOCATION (__FILE__ ":" STRINGIZE2(__LINE__))

class PfcException : public QException {
public:
    PfcException();
    PfcException(const char* location, const QString& msg);
    PfcException* clone() const override;
    void raise() const override;
    const QString toString() const;

protected:
    const char* mLocation;
    QString mMessage;
};

class Pfc : public QObject {
    Q_OBJECT

public:
    Pfc() : QObject() {}

    enum Group {
        G_None     = 0,
        G_Basic    = 1 << 0,
        G_Advanced = 1 << 1,
        G_Sensors  = 1 << 2,
        G_Aux      = 1 << 3,
    };
    Q_ENUM(Group)

    enum Id {
        Id_Unknown = 0,
        // Basic
        B_InjDuty,
        B_IGL,
        B_IGT,
        B_RPM,
        B_Speed,
        B_Boost,
        B_Knock,
        B_WtrT,
        B_AirT,
        B_BatV,
        // Advanced
        Adv_RPM,
        Adv_PIM,
        Adv_PIMV,
        Adv_TPSV,
        Adv_InjFrPr,
        Adv_InjPm,
        Adv_IGL,
        Adv_IGT,
        Adv_FuelT,
        Adv_Oil,
        Adv_PC,
        Adv_WG,
        Adv_WtrT,
        Adv_AirT,
        Adv_Knock,
        Adv_BatV,
        Adv_Speed,
        Adv_Unk1,
        Adv_O2S,
        Adv_Unk2,
        Adv_InjFrSc,
        // Sensors
        Sens_PIM,
        Sens_VTA1,
        Sens_VTA2,
        Sens_VMOP,
        Sens_WTRT,
        Sens_AIRT,
        Sens_FUEL,
        Sens_O2S,
        Sw_STR,
        Sw_AC,
        Sw_PWS,
        Sw_NTR,
        Sw_CLT,
        Sw_STP,
        Sw_CAT,
        Sw_ELD,
        Sw_HWL,
        Sw_FPD,
        Sw_FPR,
        Sw_APR,
        Sw_PAC,
        Sw_CCN,
        Sw_TCN,
        Sw_PRC,
        // Aux
        Aux1,
        Aux2,
        Aux3,
        Aux4,
        Aux5,
        Aux6,
        Aux7,
        Aux8,
        // LAST
        Id_Count
    };
    Q_ENUM(Id)

    static bool isValid(Pfc::Id id) {
        return (static_cast<int>(id) >= 0 && static_cast<int>(id) < Pfc::Id_Count);
    }
};

Q_DECLARE_METATYPE(Pfc::Group)
Q_DECLARE_METATYPE(Pfc::Id)
