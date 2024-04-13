void NumberFormatTest::expect_rbnf(NumberFormat& fmt, const UnicodeString& str, const Formattable& n) {
    UErrorCode status = U_ZERO_ERROR;
    Formattable num;
    fmt.parse(str, num, status);
    if (U_FAILURE(status)) {
        errln(UnicodeString("FAIL: Parse failed for \"") + str + "\"");
        return;
    }
    if (equalValue(num, n)) {
        logln(UnicodeString("Ok   \"") + str + " = " +
              toString(num));
    } else {
        errln(UnicodeString("FAIL \"") + str + " = " +
              toString(num) + ", expected " + toString(n));
    }
}