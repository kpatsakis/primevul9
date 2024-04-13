void NumberFormatTest::TestShowZero() {
    UErrorCode status = U_ZERO_ERROR;
    Locale locale("en_US");
    LocalPointer<DecimalFormat> numberFormat(static_cast<DecimalFormat*>(
            NumberFormat::createInstance(locale, status)));
    CHECK_DATA(status, "NumberFormat::createInstance")

    numberFormat->setSignificantDigitsUsed(TRUE);
    numberFormat->setMaximumSignificantDigits(3);

    UnicodeString result;
    numberFormat->format(0.0, result);
    if (result != "0") {
        errln((UnicodeString)"Expected: 0, got " + result);
    }
}