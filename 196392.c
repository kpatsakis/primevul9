 void NumberFormatTest::Test11647_PatternCurrencySymbols() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormat df(status);
    df.applyPattern(u"¤¤¤¤#", status);
    if (!assertSuccess("", status)) return;
    UnicodeString actual;
    df.format(123, actual);
    assertEquals("Should replace 4 currency signs with U+FFFD", u"\uFFFD123", actual);
}