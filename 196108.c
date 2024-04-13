void NumberFormatTest::Test12753_PatternDecimalPoint() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols symbols(Locale::getUS(), status);
    symbols.setSymbol(DecimalFormatSymbols::kDecimalSeparatorSymbol, u"*", false);
    DecimalFormat df(u"0.00", symbols, status);
    if (!assertSuccess("", status)) return;
    df.setDecimalPatternMatchRequired(true);
    Formattable result;
    df.parse(u"123",result, status);
    assertEquals("Parsing integer succeeded even though setDecimalPatternMatchRequired was set",
                 U_INVALID_FORMAT_ERROR, status);
    }