void NumberFormatTest::TestCurrencyFractionDigits() {
    UErrorCode status = U_ZERO_ERROR;
    UnicodeString text1, text2;
    double value = 99.12345;

    // Create currenct instance
    NumberFormat* fmt = NumberFormat::createCurrencyInstance("ja_JP", status);
    if (U_FAILURE(status) || fmt == NULL) {
        dataerrln("Unable to create NumberFormat");
    } else {
        fmt->format(value, text1);

        // Reset the same currency and format the test value again
        fmt->setCurrency(fmt->getCurrency(), status);
        ASSERT_SUCCESS(status);
        fmt->format(value, text2);

        if (text1 != text2) {
            errln((UnicodeString)"NumberFormat::format() should return the same result - text1="
                + text1 + " text2=" + text2);
        }
    }
    delete fmt;
}