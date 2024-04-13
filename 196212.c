void NumberFormatTest::verifyRounding(
        DecimalFormat& format,
        const double *values,
        const char * const *expected,
        const DecimalFormat::ERoundingMode *roundingModes,
        const char * const *descriptions,
        int32_t valueSize,
        int32_t roundingModeSize) {
    for (int32_t i = 0; i < roundingModeSize; ++i) {
        format.setRoundingMode(roundingModes[i]);
        for (int32_t j = 0; j < valueSize; j++) {
            UnicodeString currentExpected(expected[i * valueSize + j]);
            currentExpected = currentExpected.unescape();
            UnicodeString actual;
            format.format(values[j], actual);
            if (currentExpected != actual) {
                dataerrln("For %s value %f, expected '%s', got '%s'",
                          descriptions[i], values[j], CStr(currentExpected)(), CStr(actual)());
            }
        }
    }
}