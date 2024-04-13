void NumberFormatTest::expect(NumberFormat& fmt, const UnicodeString& str, const Formattable& n) {
    UErrorCode status = U_ZERO_ERROR;
    Formattable num;
    fmt.parse(str, num, status);
    if (U_FAILURE(status)) {
        dataerrln(UnicodeString("FAIL: Parse failed for \"") + str + "\" - " + u_errorName(status));
        return;
    }
    UnicodeString pat;
    ((DecimalFormat*) &fmt)->toPattern(pat);
    if (equalValue(num, n)) {
        logln(UnicodeString("Ok   \"") + str + "\" x " +
              pat + " = " +
              toString(num));
    } else {
        dataerrln(UnicodeString("FAIL \"") + str + "\" x " +
              pat + " = " +
              toString(num) + ", expected " + toString(n));
    }
}