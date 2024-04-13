void NumberFormatTest::Test11646_Equality() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols symbols(Locale::getEnglish(), status);
    UnicodeString pattern(u"\u00a4\u00a4\u00a4 0.00 %\u00a4\u00a4");
    DecimalFormat fmt(pattern, symbols, status);
    if (!assertSuccess("", status)) return;

    // Test equality with affixes. set affix methods can't capture special
    // characters which is why equality should fail.
    {
        DecimalFormat fmtCopy(fmt);
        assertTrue("", fmt == fmtCopy);
        UnicodeString positivePrefix;
        fmtCopy.setPositivePrefix(fmtCopy.getPositivePrefix(positivePrefix));
        assertFalse("", fmt == fmtCopy);
    }
    {
        DecimalFormat fmtCopy = DecimalFormat(fmt);
        assertTrue("", fmt == fmtCopy);
        UnicodeString positivePrefix;
        fmtCopy.setPositiveSuffix(fmtCopy.getPositiveSuffix(positivePrefix));
        assertFalse("", fmt == fmtCopy);
    }
    {
        DecimalFormat fmtCopy(fmt);
        assertTrue("", fmt == fmtCopy);
        UnicodeString negativePrefix;
        fmtCopy.setNegativePrefix(fmtCopy.getNegativePrefix(negativePrefix));
        assertFalse("", fmt == fmtCopy);
    }
    {
        DecimalFormat fmtCopy(fmt);
        assertTrue("", fmt == fmtCopy);
        UnicodeString negativePrefix;
        fmtCopy.setNegativeSuffix(fmtCopy.getNegativeSuffix(negativePrefix));
        assertFalse("", fmt == fmtCopy);
    }
}