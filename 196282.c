void NumberFormatTest::Test11649_DecFmtCurrencies() {
    IcuTestErrorCode status(*this, "Test11649_DecFmtCurrencies");
    UnicodeString pattern("\\u00a4\\u00a4\\u00a4 0.00");
    pattern = pattern.unescape();
    DecimalFormat fmt(pattern, status);
    if (!assertSuccess("", status, true, __FILE__, __LINE__)) { return; }
    static const UChar USD[] = u"USD";
    fmt.setCurrency(USD);
    UnicodeString appendTo;

    assertEquals("", "US dollars 12.34", fmt.format(12.34, appendTo));
    UnicodeString topattern;

    assertEquals("", pattern, fmt.toPattern(topattern));
    DecimalFormat fmt2(topattern, status);
    fmt2.setCurrency(USD);

    appendTo.remove();
    assertEquals("", "US dollars 12.34", fmt2.format(12.34, appendTo));
}