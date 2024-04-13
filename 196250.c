void NumberFormatTest::TestMismatchedCurrencyFormatFail() {
    IcuTestErrorCode status(*this, "TestMismatchedCurrencyFormatFail");
    LocalPointer<DecimalFormat> df(
            dynamic_cast<DecimalFormat*>(DecimalFormat::createCurrencyInstance("en", status)), status);
    if (!assertSuccess("createCurrencyInstance() failed.", status, true, __FILE__, __LINE__)) {return;}
    UnicodeString pattern;
    assertEquals("Test assumes that currency sign is at the beginning",
            u"\u00A4#,##0.00",
            df->toPattern(pattern));
    // Should round-trip on the correct currency format:
    expect2(*df, 1.23, u"\u00A41.23");
    df->setCurrency(u"EUR", status);
    expect2(*df, 1.23, u"\u20AC1.23");
    // Should parse with currency in the wrong place in lenient mode
    df->setLenient(TRUE);
    expect(*df, u"1.23\u20AC", 1.23);
    expectParseCurrency(*df, u"EUR", 1.23, "1.23\\u20AC");
    // Should NOT parse with currency in the wrong place in STRICT mode
    df->setLenient(FALSE);
    {
        Formattable result;
        ErrorCode failStatus;
        df->parse(u"1.23\u20AC", result, failStatus);
        assertEquals("Should fail to parse", U_INVALID_FORMAT_ERROR, failStatus);
    }
    {
        ParsePosition ppos;
        df->parseCurrency(u"1.23\u20AC", ppos);
        assertEquals("Should fail to parse currency", 0, ppos.getIndex());
    }
}