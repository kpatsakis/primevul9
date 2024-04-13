void NumberFormatTest::Test11913_BigDecimal() {
    UErrorCode status = U_ZERO_ERROR;
    LocalPointer<NumberFormat> df(NumberFormat::createInstance(Locale::getEnglish(), status), status);
    if (!assertSuccess("", status)) return;
    UnicodeString result;
    df->format(StringPiece("1.23456789E400"), result, nullptr, status);
    assertSuccess("", status);
    assertEquals("Should format more than 309 digits", u"12,345,678", UnicodeString(result, 0, 10));
    assertEquals("Should format more than 309 digits", 534, result.length());
}