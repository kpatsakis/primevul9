void NumberFormatTest::Test11020_RoundingInScientificNotation() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols sym(Locale::getFrance(), status);
    DecimalFormat fmt(u"0.05E0", sym, status);
    if (!assertSuccess("", status, true, __FILE__, __LINE__)) { return; }
    assertSuccess("", status);
    UnicodeString result;
    fmt.format(12301.2, result);
    assertEquals("Rounding increment should be applied after magnitude scaling", u"1,25E4", result);
}