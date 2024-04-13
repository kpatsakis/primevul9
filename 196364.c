void NumberFormatTest::Test13777_ParseLongNameNonCurrencyMode() {
    IcuTestErrorCode status(*this, "Test13777_ParseLongNameNonCurrencyMode");

    LocalPointer<NumberFormat> df(
        NumberFormat::createInstance("en-us", UNumberFormatStyle::UNUM_CURRENCY_PLURAL, status), status);
    if (!assertSuccess("", status, true, __FILE__, __LINE__)) { return; }
    expect2(*df, 1.5, u"1.50 US dollars");
}