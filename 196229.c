void NumberFormatTest::TestWhiteSpaceParsing(void) {
    UErrorCode ec = U_ZERO_ERROR;
    DecimalFormatSymbols US(Locale::getUS(), ec);
    DecimalFormat fmt("a  b#0c  ", US, ec);
    if (U_FAILURE(ec)) {
        errcheckln(ec, "FAIL: Constructor - %s", u_errorName(ec));
        return;
    }
    // From ICU 62, flexible whitespace needs lenient mode
    fmt.setLenient(TRUE);
    int32_t n = 1234;
    expect(fmt, "a b1234c ", n);
    expect(fmt, "a   b1234c   ", n);
}