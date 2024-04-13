void NumberFormatTest::TestFractionalDigitsForCurrency() {
    UErrorCode status = U_ZERO_ERROR;
    LocalPointer<NumberFormat> fmt(NumberFormat::createCurrencyInstance("en", status));
    if (U_FAILURE(status)) {
        dataerrln("Error creating NumberFormat - %s", u_errorName(status));
        return;
    }
    UChar JPY[] = {0x4A, 0x50, 0x59, 0x0};
    fmt->setCurrency(JPY, status);
    if (!assertSuccess("", status)) {
        return;
    }
    assertEquals("", 0, fmt->getMaximumFractionDigits());
}