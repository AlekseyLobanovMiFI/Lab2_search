#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <string>

inline int HashInt(const int& key) {//разрешает подключение двух разных .cpp к .h файлу
    // перемешивание, чтобы подряд идущие ключи не ложились подряд
    unsigned x = (unsigned)key;
    x ^= x >> 16;
    x *= 0x45d9f3bu;
    x ^= x >> 16;
    return (int)(x & 0x7fffffffu);
}

inline int HashString(const std::string& key) {
    // полиномиальный хеш: h = h * 31 + c
    unsigned h = 0;
    for (unsigned char c : key) {
        h = h * 31u + c;
    }
    return (int)(h & 0x7fffffffu);
}

#endif