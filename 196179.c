void NumberFormatTest::expect(NumberFormat* fmt, const Formattable& n,
                              const UnicodeString& exp, UBool rt,
                              UErrorCode status) {
    if (fmt == NULL || U_FAILURE(status)) {
        dataerrln("FAIL: NumberFormat constructor");
    } else {
        expect(*fmt, n, exp, rt);
    }
    delete fmt;
}