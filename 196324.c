void NumberFormatTest::Test20073_StrictPercentParseErrorIndex() {
    IcuTestErrorCode status(*this, "Test20073_StrictPercentParseErrorIndex");
    ParsePosition parsePosition(0);
    DecimalFormat df(u"0%", {"en-us", status}, status);
    if (U_FAILURE(status)) {
        dataerrln("Unable to create DecimalFormat instance.");
        return;
    }
    df.setLenient(FALSE);
    Formattable result;
    df.parse(u"%2%", result, parsePosition);
    assertEquals("", 0, parsePosition.getIndex());
    assertEquals("", 0, parsePosition.getErrorIndex());
}