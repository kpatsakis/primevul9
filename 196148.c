void NumberFormatTest::Test11640_getAffixes() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols symbols("en_US", status);
    if (!assertSuccess("", status)) {
        return;
    }
    UnicodeString pattern("\\u00a4\\u00a4\\u00a4 0.00 %\\u00a4\\u00a4");
    pattern = pattern.unescape();
    DecimalFormat fmt(pattern, symbols, status);
    if (!assertSuccess("", status)) {
        return;
    }
    UnicodeString affixStr;
    assertEquals("", "US dollars ", fmt.getPositivePrefix(affixStr));
    assertEquals("", " %USD", fmt.getPositiveSuffix(affixStr));
    assertEquals("", "-US dollars ", fmt.getNegativePrefix(affixStr));
    assertEquals("", " %USD", fmt.getNegativeSuffix(affixStr));
}