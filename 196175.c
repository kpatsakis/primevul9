void NumberFormatTest::Test11649_toPatternWithMultiCurrency() {
    UnicodeString pattern("\\u00a4\\u00a4\\u00a4 0.00");
    pattern = pattern.unescape();
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormat fmt(pattern, status);
    if (!assertSuccess("", status)) {
        return;
    }
    static UChar USD[] = {0x55, 0x53, 0x44, 0x0};
    fmt.setCurrency(USD);
    UnicodeString appendTo;

    assertEquals("", "US dollars 12.34", fmt.format(12.34, appendTo));

    UnicodeString topattern;
    fmt.toPattern(topattern);
    DecimalFormat fmt2(topattern, status);
    if (!assertSuccess("", status)) {
        return;
    }
    fmt2.setCurrency(USD);

    appendTo.remove();
    assertEquals("", "US dollars 12.34", fmt2.format(12.34, appendTo));
}