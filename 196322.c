void NumberFormatTest::TestExponent(void) {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols US(Locale::getUS(), status);
    CHECK(status, "DecimalFormatSymbols constructor");
    DecimalFormat fmt1(UnicodeString("0.###E0"), US, status);
    CHECK(status, "DecimalFormat(0.###E0)");
    DecimalFormat fmt2(UnicodeString("0.###E+0"), US, status);
    CHECK(status, "DecimalFormat(0.###E+0)");
    int32_t n = 1234;
    expect2(fmt1, n, "1.234E3");
    expect2(fmt2, n, "1.234E+3");
    expect(fmt1, "1.234E+3", n); // Either format should parse "E+3"
}