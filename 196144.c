void NumberFormatTest::TestGetAffixes() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols sym("en_US", status);
    UnicodeString pattern("\\u00a4\\u00a4\\u00a4 0.00 %\\u00a4\\u00a4");
    pattern = pattern.unescape();
    DecimalFormat fmt(pattern, sym, status);
    if (U_FAILURE(status)) {
        dataerrln("Error creating DecimalFormat - %s", u_errorName(status));
        return;
    }
    UnicodeString affixStr;
    assertEquals("", "US dollars ", fmt.getPositivePrefix(affixStr));
    assertEquals("", " %USD", fmt.getPositiveSuffix(affixStr));
    assertEquals("", "-US dollars ", fmt.getNegativePrefix(affixStr));
    assertEquals("", " %USD", fmt.getNegativeSuffix(affixStr));

    // Test equality with affixes. set affix methods can't capture special
    // characters which is why equality should fail.
    {
        DecimalFormat fmtCopy(fmt);
        assertTrue("", fmt == fmtCopy);
        UnicodeString someAffix;
        fmtCopy.setPositivePrefix(fmtCopy.getPositivePrefix(someAffix));
        assertTrue("", fmt != fmtCopy);
    }
    {
        DecimalFormat fmtCopy(fmt);
        assertTrue("", fmt == fmtCopy);
        UnicodeString someAffix;
        fmtCopy.setPositiveSuffix(fmtCopy.getPositiveSuffix(someAffix));
        assertTrue("", fmt != fmtCopy);
    }
    {
        DecimalFormat fmtCopy(fmt);
        assertTrue("", fmt == fmtCopy);
        UnicodeString someAffix;
        fmtCopy.setNegativePrefix(fmtCopy.getNegativePrefix(someAffix));
        assertTrue("", fmt != fmtCopy);
    }
    {
        DecimalFormat fmtCopy(fmt);
        assertTrue("", fmt == fmtCopy);
        UnicodeString someAffix;
        fmtCopy.setNegativeSuffix(fmtCopy.getNegativeSuffix(someAffix));
        assertTrue("", fmt != fmtCopy);
    }
    fmt.setPositivePrefix("Don't");
    fmt.setPositiveSuffix("do");
    UnicodeString someAffix("be''eet\\u00a4\\u00a4\\u00a4 it.");
    someAffix = someAffix.unescape();
    fmt.setNegativePrefix(someAffix);
    fmt.setNegativeSuffix("%");
    assertEquals("", "Don't", fmt.getPositivePrefix(affixStr));
    assertEquals("", "do", fmt.getPositiveSuffix(affixStr));
    assertEquals("", someAffix, fmt.getNegativePrefix(affixStr));
    assertEquals("", "%", fmt.getNegativeSuffix(affixStr));
}