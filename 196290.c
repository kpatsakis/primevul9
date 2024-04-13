void NumberFormatTest::TestParseNaN() {
    IcuTestErrorCode status(*this, "TestParseNaN");

    DecimalFormat df("0", { "en", status }, status);
    if (!assertSuccess("", status, true, __FILE__, __LINE__)) { return; }
    Formattable parseResult;
    df.parse(u"NaN", parseResult, status);
    assertEquals("NaN should parse successfully", NAN, parseResult.getDouble());
    assertFalse("Result NaN should be positive", std::signbit(parseResult.getDouble()));
    UnicodeString formatResult;
    df.format(parseResult.getDouble(), formatResult);
    assertEquals("NaN should round-trip", u"NaN", formatResult);
}