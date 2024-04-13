void NumberFormatTest::Test12567() {
    IcuTestErrorCode errorCode(*this, "Test12567");
    // Ticket #12567: DecimalFormat.equals() may not be symmetric
    LocalPointer<DecimalFormat> df1((DecimalFormat *)
        NumberFormat::createInstance(Locale::getUS(), UNUM_CURRENCY, errorCode));
    LocalPointer<DecimalFormat> df2((DecimalFormat *)
        NumberFormat::createInstance(Locale::getUS(), UNUM_DECIMAL, errorCode));
    if (!assertSuccess("", errorCode, true, __FILE__, __LINE__)) { return; }
    // NOTE: CurrencyPluralInfo equality not tested in C++ because its operator== is not defined.
    df1->applyPattern(u"0.00", errorCode);
    df2->applyPattern(u"0.00", errorCode);
    assertTrue("df1 == df2", *df1 == *df2);
    assertTrue("df2 == df1", *df2 == *df1);
    df2->setPositivePrefix(u"abc");
    assertTrue("df1 != df2", *df1 != *df2);
    assertTrue("df2 != df1", *df2 != *df1);
}