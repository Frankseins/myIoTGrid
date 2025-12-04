/**
 * ArduinoJsonString.h - ArduinoJson converters for native String class
 * Include this AFTER ArduinoJson.h in source files that need JSON support
 */

#ifndef ARDUINOJSON_STRING_H
#define ARDUINOJSON_STRING_H

#ifdef PLATFORM_NATIVE

#include <ArduinoJson.h>
#include "Arduino.h"

namespace ArduinoJson {

/**
 * Converter for native String class to/from JSON
 */
template <>
struct Converter<String> {
    static void toJson(const String& src, JsonVariant dst) {
        dst.set(src.c_str());
    }

    static String fromJson(JsonVariantConst src) {
        const char* str = src.as<const char*>();
        return str ? String(str) : String();
    }

    static bool checkJson(JsonVariantConst src) {
        return src.is<const char*>();
    }
};

} // namespace ArduinoJson

#endif // PLATFORM_NATIVE

#endif // ARDUINOJSON_STRING_H
