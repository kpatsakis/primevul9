NumberFormatTest::TestRounding487(void)
{
    UErrorCode status = U_ZERO_ERROR;
    NumberFormat *nf = NumberFormat::createInstance(status);
    if (U_FAILURE(status)) {
        dataerrln("Error calling NumberFormat::createInstance()");
        return;
    }

    roundingTest(*nf, 0.00159999, 4, "0.0016");
    roundingTest(*nf, 0.00995, 4, "0.01");

    roundingTest(*nf, 12.3995, 3, "12.4");

    roundingTest(*nf, 12.4999, 0, "12");
    roundingTest(*nf, - 19.5, 0, "-20");
    delete nf;
    if (U_FAILURE(status)) errln((UnicodeString)"FAIL: Status " + (int32_t)status);
}