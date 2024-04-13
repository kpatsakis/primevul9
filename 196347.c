void NumberFormatTest::Test13391_chakmaParsing() {
    UErrorCode status = U_ZERO_ERROR;
    LocalPointer<DecimalFormat> df(dynamic_cast<DecimalFormat*>(
        NumberFormat::createInstance(Locale("ccp"), status)));
    if (df == nullptr) {
        dataerrln("%s %d Chakma df is null",  __FILE__, __LINE__);
        return;
    }
    const UChar* expected = u"\U00011137\U00011138,\U00011139\U0001113A\U0001113B";
    UnicodeString actual;
    df->format(12345, actual, status);
    assertSuccess("Should not fail when formatting in ccp", status);
    assertEquals("Should produce expected output in ccp", expected, actual);

    Formattable result;
    df->parse(expected, result, status);
    assertSuccess("Should not fail when parsing in ccp", status);
    assertEquals("Should parse to 12345 in ccp", 12345, result);

    const UChar* expectedScientific = u"\U00011137.\U00011139E\U00011138";
    UnicodeString actualScientific;
    df.adoptInstead(static_cast<DecimalFormat*>(
        NumberFormat::createScientificInstance(Locale("ccp"), status)));
    df->format(130, actualScientific, status);
    assertSuccess("Should not fail when formatting scientific in ccp", status);
    assertEquals("Should produce expected scientific output in ccp",
        expectedScientific, actualScientific);

    Formattable resultScientific;
    df->parse(expectedScientific, resultScientific, status);
    assertSuccess("Should not fail when parsing scientific in ccp", status);
    assertEquals("Should parse scientific to 130 in ccp", 130, resultScientific);
}