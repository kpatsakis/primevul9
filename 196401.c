void NumberFormatTest::TestToPatternScientific11648() {
    UErrorCode status = U_ZERO_ERROR;
    Locale en("en");
    DecimalFormatSymbols sym(en, status);
    DecimalFormat fmt("0.00", sym, status);
    if (U_FAILURE(status)) {
        dataerrln("Error creating DecimalFormat - %s", u_errorName(status));
        return;
    }
    fmt.setScientificNotation(TRUE);
    UnicodeString pattern;
    assertEquals("", "0.00E0", fmt.toPattern(pattern));
    DecimalFormat fmt2(pattern, sym, status);
    assertSuccess("", status);
}