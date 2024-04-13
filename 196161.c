void NumberFormatTest::TestParseNegativeWithAlternateMinusSign() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormat *test = (DecimalFormat *) NumberFormat::createInstance("en", status);
    CHECK_DATA(status, "NumberFormat::createInstance")
    test->setLenient(TRUE);
    Formattable af;
    ParsePosition ppos;
    UnicodeString value("\\u208B0.5");
    value = value.unescape();
    test->parse(value, af, ppos);
    if (ppos.getIndex() == 0) {
        errln(UnicodeString("Expected ") + value + UnicodeString(" to parse."));
    }
    delete test;
}