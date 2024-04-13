void NumberFormatTest::TestParsePercentRegression() {
    IcuTestErrorCode status(*this, "TestParsePercentRegression");
    LocalPointer<DecimalFormat> df1((DecimalFormat*) NumberFormat::createInstance("en", status), status);
    LocalPointer<DecimalFormat> df2((DecimalFormat*) NumberFormat::createPercentInstance("en", status), status);
    if (status.isFailure()) {return; }
    df1->setLenient(TRUE);
    df2->setLenient(TRUE);

    {
        ParsePosition ppos;
        Formattable result;
        df1->parse("50%", result, ppos);
        assertEquals("df1 should accept a number but not the percent sign", 2, ppos.getIndex());
        assertEquals("df1 should return the number as 50", 50.0, result.getDouble(status));
    }
    {
        ParsePosition ppos;
        Formattable result;
        df2->parse("50%", result, ppos);
        assertEquals("df2 should accept the percent sign", 3, ppos.getIndex());
        assertEquals("df2 should return the number as 0.5", 0.5, result.getDouble(status));
    }
    {
        ParsePosition ppos;
        Formattable result;
        df2->parse("50", result, ppos);
        assertEquals("df2 should return the number as 0.5 even though the percent sign is missing",
                0.5,
                result.getDouble(status));
    }
}