#pragma once

#include <stdint.h>

#pragma pack(push, 1)

struct PfcRawBasic {
    uint8_t Id;
    uint8_t Len;
    uint16_t InjDuty;
    uint16_t IGL;
    uint16_t IGT;
    uint16_t RPM;
    uint16_t Speed;
    uint16_t Boost;
    uint16_t Knock;
    uint16_t WtrT;
    uint16_t AirT;
    uint16_t BatV;
    uint8_t Crc;
};

struct PfcRawAdvanced {
    uint8_t Id;
    uint8_t Len;
    uint16_t RPM;
    uint16_t PIM;
    uint16_t PIMV;
    uint16_t TPSV;
    uint16_t InjFrPr;
    uint16_t InjPm;
    uint8_t IGL;
    uint8_t IGT;
    uint8_t FuelT;
    uint8_t Oil;
    uint8_t PC;
    uint8_t WG;
    uint8_t WtrT;
    uint8_t AirT;
    uint8_t Knock;
    uint8_t BatV;
    uint16_t Speed;
    uint16_t Unk1;
    uint8_t O2S;
    uint8_t Unk2;
    uint16_t InjFrSc;
    uint8_t Crc;
};

struct PfcRawSensors {
    uint8_t Id;
    uint8_t Len;
    uint16_t PIM;
    uint16_t VTA1;
    uint16_t VTA2;
    uint16_t VMOP;
    uint16_t WTRT;
    uint16_t AIRT;
    uint16_t FUEL;
    uint16_t O2S;
    uint16_t Flags;
    uint8_t Crc;
};

struct PfcRawAux {
    uint8_t Id;
    uint8_t Len;
    uint16_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint16_t Data4;
    uint16_t Data5;
    uint16_t Data6;
    uint16_t Data7;
    uint16_t Data8;
    uint8_t Crc;
};

#pragma pack(pop)
