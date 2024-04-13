void NumberFormatTest::Test11376_getAndSetPositivePrefix() {
    {
        const UChar USD[] = {0x55, 0x53, 0x44, 0x0};
        UErrorCode status = U_ZERO_ERROR;
        LocalPointer<NumberFormat> fmt(
                NumberFormat::createCurrencyInstance("en", status));
        if (!assertSuccess("", status)) {
            return;
        }
        DecimalFormat *dfmt = (DecimalFormat *) fmt.getAlias();
        dfmt->setCurrency(USD);
        UnicodeString result;

        // This line should be a no-op. I am setting the positive prefix
        // to be the same thing it was before.
        dfmt->setPositivePrefix(dfmt->getPositivePrefix(result));

        UnicodeString appendTo;
        assertEquals("", "$3.78", dfmt->format(3.78, appendTo, status));
        assertSuccess("", status);
    }
    {
        const UChar USD[] = {0x55, 0x53, 0x44, 0x0};
        UErrorCode status = U_ZERO_ERROR;
        LocalPointer<NumberFormat> fmt(
                NumberFormat::createInstance("en", UNUM_CURRENCY_PLURAL, status));
        if (!assertSuccess("", status)) {
            return;
        }
        DecimalFormat *dfmt = (DecimalFormat *) fmt.getAlias();
        UnicodeString result;
        assertEquals("", u" (unknown currency)", dfmt->getPositiveSuffix(result));
        dfmt->setCurrency(USD);

        // getPositiveSuffix() always returns the suffix for the
        // "other" plural category
        assertEquals("", " US dollars", dfmt->getPositiveSuffix(result));
        UnicodeString appendTo;
        assertEquals("", "3.78 US dollars", dfmt->format(3.78, appendTo, status));
        assertEquals("", " US dollars", dfmt->getPositiveSuffix(result));
        dfmt->setPositiveSuffix("booya");
        appendTo.remove();
        assertEquals("", "3.78booya", dfmt->format(3.78, appendTo, status));
        assertEquals("", "booya", dfmt->getPositiveSuffix(result));
    }
}