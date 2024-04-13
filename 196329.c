void NumberFormatTest::Test11868() {
    double posAmt = 34.567;
    double negAmt = -9876.543;

    Locale selectedLocale("en_US");
    UErrorCode status = U_ZERO_ERROR;

    UnicodeString result;
    FieldPosition fpCurr(UNUM_CURRENCY_FIELD);
    LocalPointer<NumberFormat> fmt(
            NumberFormat::createInstance(
                    selectedLocale, UNUM_CURRENCY_PLURAL, status));
    if (!assertSuccess("Format creation", status)) {
        return;
    }
    fmt->format(posAmt, result, fpCurr, status);
    assertEquals("", "34.57 US dollars", result);
    assertEquals("begin index", 6, fpCurr.getBeginIndex());
    assertEquals("end index", 16, fpCurr.getEndIndex());

    // Test field position iterator
    {
        NumberFormatTest_Attributes attributes[] = {
                {UNUM_INTEGER_FIELD, 0, 2},
                {UNUM_DECIMAL_SEPARATOR_FIELD, 2, 3},
                {UNUM_FRACTION_FIELD, 3, 5},
                {UNUM_CURRENCY_FIELD, 6, 16},
                {0, -1, 0}};
        UnicodeString result;
        FieldPositionIterator iter;
        fmt->format(posAmt, result, &iter, status);
        assertEquals("", "34.57 US dollars", result);
        verifyFieldPositionIterator(attributes, iter);
    }

    result.remove();
    fmt->format(negAmt, result, fpCurr, status);
    assertEquals("", "-9,876.54 US dollars", result);
    assertEquals("begin index", 10, fpCurr.getBeginIndex());
    assertEquals("end index", 20, fpCurr.getEndIndex());

    // Test field position iterator
    {
        NumberFormatTest_Attributes attributes[] = {
                {UNUM_SIGN_FIELD, 0, 1},
                {UNUM_GROUPING_SEPARATOR_FIELD, 2, 3},
                {UNUM_INTEGER_FIELD, 1, 6},
                {UNUM_DECIMAL_SEPARATOR_FIELD, 6, 7},
                {UNUM_FRACTION_FIELD, 7, 9},
                {UNUM_CURRENCY_FIELD, 10, 20},
                {0, -1, 0}};
        UnicodeString result;
        FieldPositionIterator iter;
        fmt->format(negAmt, result, &iter, status);
        assertEquals("", "-9,876.54 US dollars", result);
        verifyFieldPositionIterator(attributes, iter);
    }
}