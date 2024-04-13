void NumberFormatTest::expect_rbnf(NumberFormat& fmt, const Formattable& n,
                              const UnicodeString& exp, UBool rt) {
    UnicodeString saw;
    FieldPosition pos;
    UErrorCode status = U_ZERO_ERROR;
    fmt.format(n, saw, pos, status);
    CHECK(status, "NumberFormat::format");
    if (saw == exp) {
        logln(UnicodeString("Ok   ") + toString(n) +
              " = \"" +
              escape(saw) + "\"");
        // We should be able to round-trip the formatted string =>
        // number => string (but not the other way around: number
        // => string => number2, might have number2 != number):
        if (rt) {
            Formattable n2;
            fmt.parse(exp, n2, status);
            if (U_FAILURE(status)) {
                errln(UnicodeString("FAIL: Parse failed for \"") + exp + "\"");
                return;
            }
            UnicodeString saw2;
            fmt.format(n2, saw2, pos, status);
            CHECK(status, "NumberFormat::format");
            if (saw2 != exp) {
                errln((UnicodeString)"FAIL \"" + exp + "\" => " + toString(n2) +
                      " => \"" + saw2 + "\"");
            }
        }
    } else {
        errln(UnicodeString("FAIL ") + toString(n) +
              " = \"" +
              escape(saw) + "\", expected \"" + exp + "\"");
    }
}