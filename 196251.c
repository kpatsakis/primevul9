void NumberFormatTest::TestBug9936() {
    UErrorCode status = U_ZERO_ERROR;
    Locale locale("en_US");
    LocalPointer<DecimalFormat> numberFormat(static_cast<DecimalFormat*>(
            NumberFormat::createInstance(locale, status)));
    if (U_FAILURE(status)) {
        dataerrln("File %s, Line %d: status = %s.\n", __FILE__, __LINE__, u_errorName(status));
        return;
    }

    if (numberFormat->areSignificantDigitsUsed() == TRUE) {
        errln("File %s, Line %d: areSignificantDigitsUsed() was TRUE, expected FALSE.\n", __FILE__, __LINE__);
    }
    numberFormat->setSignificantDigitsUsed(TRUE);
    if (numberFormat->areSignificantDigitsUsed() == FALSE) {
        errln("File %s, Line %d: areSignificantDigitsUsed() was FALSE, expected TRUE.\n", __FILE__, __LINE__);
    }

    numberFormat->setSignificantDigitsUsed(FALSE);
    if (numberFormat->areSignificantDigitsUsed() == TRUE) {
        errln("File %s, Line %d: areSignificantDigitsUsed() was TRUE, expected FALSE.\n", __FILE__, __LINE__);
    }

    numberFormat->setMinimumSignificantDigits(3);
    if (numberFormat->areSignificantDigitsUsed() == FALSE) {
        errln("File %s, Line %d: areSignificantDigitsUsed() was FALSE, expected TRUE.\n", __FILE__, __LINE__);
    }

    numberFormat->setSignificantDigitsUsed(FALSE);
    numberFormat->setMaximumSignificantDigits(6);
    if (numberFormat->areSignificantDigitsUsed() == FALSE) {
        errln("File %s, Line %d: areSignificantDigitsUsed() was FALSE, expected TRUE.\n", __FILE__, __LINE__);
    }

}