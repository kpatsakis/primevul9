void NumberFormatTest::TestCtorApplyPatternDifference() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols sym("en_US", status);
    UnicodeString pattern("\\u00a40");
    DecimalFormat fmt(pattern.unescape(), sym, status);
    if (U_FAILURE(status)) {
        dataerrln("Error creating DecimalFormat - %s", u_errorName(status));
        return;
    }
    UnicodeString result;
    assertEquals(
            "ctor favors precision of currency",
            "$5.00",
            fmt.format((double)5, result));
    result.remove();
    fmt.applyPattern(pattern.unescape(), status);
    assertEquals(
            "applyPattern favors precision of pattern",
            "$5",
            fmt.format((double)5, result));
}