void NumberFormatTest::Test11475_signRecognition() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols sym("en", status);
    UnicodeString result;
    {
        DecimalFormat fmt("+0.00", sym, status);
        if (!assertSuccess("", status)) {
            return;
        }
        NumberFormatTest_Attributes attributes[] = {
                {UNUM_SIGN_FIELD, 0, 1},
                {UNUM_INTEGER_FIELD, 1, 2},
                {UNUM_DECIMAL_SEPARATOR_FIELD, 2, 3},
                {UNUM_FRACTION_FIELD, 3, 5},
                {0, -1, 0}};
        UnicodeString result;
        FieldPositionIterator iter;
        fmt.format(2.3, result, &iter, status);
        assertEquals("", "+2.30", result);
        verifyFieldPositionIterator(attributes, iter);
    }
    {
        DecimalFormat fmt("++0.00+;-(#)--", sym, status);
        if (!assertSuccess("", status)) {
            return;
        }
        {
            NumberFormatTest_Attributes attributes[] = {
                    {UNUM_SIGN_FIELD, 0, 2},
                    {UNUM_INTEGER_FIELD, 2, 3},
                    {UNUM_DECIMAL_SEPARATOR_FIELD, 3, 4},
                    {UNUM_FRACTION_FIELD, 4, 6},
                    {UNUM_SIGN_FIELD, 6, 7},
                    {0, -1, 0}};
            UnicodeString result;
            FieldPositionIterator iter;
            fmt.format(2.3, result, &iter, status);
            assertEquals("", "++2.30+", result);
            verifyFieldPositionIterator(attributes, iter);
        }
        {
            NumberFormatTest_Attributes attributes[] = {
                    {UNUM_SIGN_FIELD, 0, 1},
                    {UNUM_INTEGER_FIELD, 2, 3},
                    {UNUM_DECIMAL_SEPARATOR_FIELD, 3, 4},
                    {UNUM_FRACTION_FIELD, 4, 6},
                    {UNUM_SIGN_FIELD, 7, 9},
                    {0, -1, 0}};
            UnicodeString result;
            FieldPositionIterator iter;
            fmt.format(-2.3, result, &iter, status);
            assertEquals("", "-(2.30)--", result);
            verifyFieldPositionIterator(attributes, iter);
        }
    }
}