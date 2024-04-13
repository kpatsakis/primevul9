void NumberFormatTest::expect2(NumberFormat* fmt, const Formattable& n,
                               const UnicodeString& exp,
                               UErrorCode status) {
    if (fmt == NULL || U_FAILURE(status)) {
        dataerrln("FAIL: NumberFormat constructor");
    } else {
        expect2(*fmt, n, exp);
    }
    delete fmt;
}