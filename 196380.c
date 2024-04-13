void NumberFormatTest::TestSignificantDigits(void) {
  double input[] = {
        0, 0,
        0.1, -0.1,
        123, -123,
        12345, -12345,
        123.45, -123.45,
        123.44501, -123.44501,
        0.001234, -0.001234,
        0.00000000123, -0.00000000123,
        0.0000000000000000000123, -0.0000000000000000000123,
        1.2, -1.2,
        0.0000000012344501, -0.0000000012344501,
        123445.01, -123445.01,
        12344501000000000000000000000000000.0, -12344501000000000000000000000000000.0,
    };
    const char* expected[] = {
        "0.00", "0.00",
        "0.100", "-0.100",
        "123", "-123",
        "12345", "-12345",
        "123.45", "-123.45",
        "123.45", "-123.45",
        "0.001234", "-0.001234",
        "0.00000000123", "-0.00000000123",
        "0.0000000000000000000123", "-0.0000000000000000000123",
        "1.20", "-1.20",
        "0.0000000012345", "-0.0000000012345",
        "123450", "-123450",
        "12345000000000000000000000000000000", "-12345000000000000000000000000000000",
    };

    UErrorCode status = U_ZERO_ERROR;
    Locale locale("en_US");
    LocalPointer<DecimalFormat> numberFormat(static_cast<DecimalFormat*>(
            NumberFormat::createInstance(locale, status)));
    CHECK_DATA(status,"NumberFormat::createInstance")

    numberFormat->setSignificantDigitsUsed(TRUE);
    numberFormat->setMinimumSignificantDigits(3);
    numberFormat->setMaximumSignificantDigits(5);
    numberFormat->setGroupingUsed(false);

    UnicodeString result;
    UnicodeString expectedResult;
    for (unsigned int i = 0; i < UPRV_LENGTHOF(input); ++i) {
        numberFormat->format(input[i], result);
        UnicodeString expectedResult(expected[i]);
        if (result != expectedResult) {
          errln((UnicodeString)"Expected: '" + expectedResult + "' got '" + result);
        }
        result.remove();
    }

    // Test for ICU-20063
    {
        DecimalFormat df({"en-us", status}, status);
        df.setSignificantDigitsUsed(TRUE);
        expect(df, 9.87654321, u"9.87654");
        df.setMaximumSignificantDigits(3);
        expect(df, 9.87654321, u"9.88");
        // setSignificantDigitsUsed with maxSig only
        df.setSignificantDigitsUsed(TRUE);
        expect(df, 9.87654321, u"9.88");
        df.setMinimumSignificantDigits(2);
        expect(df, 9, u"9.0");
        // setSignificantDigitsUsed with both minSig and maxSig
        df.setSignificantDigitsUsed(TRUE);
        expect(df, 9, u"9.0");
        // setSignificantDigitsUsed to false: should revert to fraction rounding
        df.setSignificantDigitsUsed(FALSE);
        expect(df, 9.87654321, u"9.876543");
        expect(df, 9, u"9");
        df.setSignificantDigitsUsed(TRUE);
        df.setMinimumSignificantDigits(2);
        expect(df, 9.87654321, u"9.87654");
        expect(df, 9, u"9.0");
        // setSignificantDigitsUsed with minSig only
        df.setSignificantDigitsUsed(TRUE);
        expect(df, 9.87654321, u"9.87654");
        expect(df, 9, u"9.0");
    }
}