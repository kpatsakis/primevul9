void NumberFormatTest::TestCompatibleCurrencies() {
/*
    static const UChar JPY[] = {0x4A, 0x50, 0x59, 0};
    static const UChar CNY[] = {0x43, 0x4E, 0x59, 0};
    UErrorCode status = U_ZERO_ERROR;
    LocalPointer<NumberFormat> fmt(
        NumberFormat::createCurrencyInstance(Locale::getUS(), status));
    if (U_FAILURE(status)) {
        errln("Could not create number format instance.");
        return;
    }
    logln("%s:%d - testing parse of halfwidth yen sign\n", __FILE__, __LINE__);
    expectParseCurrency(*fmt, JPY, 1235,  "\\u00A51,235");
    logln("%s:%d - testing parse of fullwidth yen sign\n", __FILE__, __LINE__);
    expectParseCurrency(*fmt, JPY, 1235,  "\\uFFE51,235");
    logln("%s:%d - testing parse of halfwidth yen sign\n", __FILE__, __LINE__);
    expectParseCurrency(*fmt, CNY, 1235,  "CN\\u00A51,235");

    LocalPointer<NumberFormat> fmtTW(
        NumberFormat::createCurrencyInstance(Locale::getTaiwan(), status));

    logln("%s:%d - testing parse of halfwidth yen sign in TW\n", __FILE__, __LINE__);
    expectParseCurrency(*fmtTW, CNY, 1235,  "\\u00A51,235");
    logln("%s:%d - testing parse of fullwidth yen sign in TW\n", __FILE__, __LINE__);
    expectParseCurrency(*fmtTW, CNY, 1235,  "\\uFFE51,235");

    LocalPointer<NumberFormat> fmtJP(
        NumberFormat::createCurrencyInstance(Locale::getJapan(), status));

    logln("%s:%d - testing parse of halfwidth yen sign in JP\n", __FILE__, __LINE__);
    expectParseCurrency(*fmtJP, JPY, 1235,  "\\u00A51,235");
    logln("%s:%d - testing parse of fullwidth yen sign in JP\n", __FILE__, __LINE__);
    expectParseCurrency(*fmtJP, JPY, 1235,  "\\uFFE51,235");

    // more..
*/
}