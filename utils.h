#pragma once

#include <QtGlobal>
#include <QtDebug>

namespace ut {

QString osGetEnv(const QString& name, const QString& defValue);
void osPowerOff();

// random

inline double random(double max) {
    return static_cast<double>(qrand()) / ((static_cast<double>(RAND_MAX) + 1.0) / max);
}

inline double random(double min, double max) {
    return min + random(max - min);
}

inline float randomf(float max) {
    return static_cast<float>(qrand()) / ((static_cast<float>(RAND_MAX) + 1.0f) / max);
}

inline float randomf(float min, float max) {
    return min + randomf(max - min);
}

// bit hacks

inline bool getbit(quint16 value, int id) {
    return (value & (1 << id)) != 0;
}

inline uint16_t packF16(float value, float min, float max) {
    const float maxU16 = 65535.0f;
    const float packed = maxU16 * (value - min) / (max - min);
    return static_cast<uint16_t>(packed < 0.0f ? 0.0f : (packed > maxU16 ? maxU16 : packed));
}

inline float unpackF16(uint16_t packed, float min, float max) {
    const float maxU16 = 65535.0f;
    const float value = min + (static_cast<float>(packed) / maxU16) * (max - min);
    return (value < min ? min : (value > max ? max : value));
}

} // ut
