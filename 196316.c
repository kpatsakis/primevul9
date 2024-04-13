void NumberFormatTest::TestParseNegativeWithFaLocale() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormat *test = (DecimalFormat *) NumberFormat::createInstance("fa", status);
    CHECK_DATA(status, "NumberFormat::createInstance")
    test->setLenient(TRUE);
    Formattable af;
    ParsePosition ppos;
    UnicodeString value("\\u200e-0,5");
    value = value.unescape();
    test->parse(value, af, ppos);
    if (ppos.getIndex() == 0) {
        errln("Expected -0,5 to parse for Farsi.");
    }
    delete test;
}