void NumberFormatTest::Test11025_CurrencyPadding() {
    UErrorCode status = U_ZERO_ERROR;
    UnicodeString pattern(u"¤¤ **####0.00");
    DecimalFormatSymbols sym(Locale::getFrance(), status);
    if (!assertSuccess("", status)) return;
    DecimalFormat fmt(pattern, sym, status);
    if (!assertSuccess("", status)) return;
    UnicodeString result;
    fmt.format(433.0, result);
    assertEquals("Number should be padded to 11 characters", "EUR *433,00", result);
}