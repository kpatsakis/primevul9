void NumberFormatTest::Test13737_ParseScientificStrict() {
    IcuTestErrorCode status(*this, "Test13737_ParseScientificStrict");
    LocalPointer<NumberFormat> df(NumberFormat::createScientificInstance("en", status), status);
    if (!assertSuccess("", status, true, __FILE__, __LINE__)) {return;}
    df->setLenient(FALSE);
    // Parse Test
    expect(*df, u"1.2", 1.2);
}