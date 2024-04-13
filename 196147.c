void NumberFormatTest::Test11839() {
    IcuTestErrorCode errorCode(*this, "Test11839");
    // Ticket #11839: DecimalFormat does not respect custom plus sign
    LocalPointer<DecimalFormatSymbols> dfs(new DecimalFormatSymbols(Locale::getEnglish(), errorCode), errorCode);
    if (!assertSuccess("", errorCode, true, __FILE__, __LINE__)) { return; }
    dfs->setSymbol(DecimalFormatSymbols::kMinusSignSymbol, u"a∸");
    dfs->setSymbol(DecimalFormatSymbols::kPlusSignSymbol, u"b∔"); //  ∔  U+2214 DOT PLUS
    DecimalFormat df(u"0.00+;0.00-", dfs.orphan(), errorCode);
    UnicodeString result;
    df.format(-1.234, result, errorCode);
    assertEquals("Locale-specific minus sign should be used", u"1.23a∸", result);
    df.format(1.234, result.remove(), errorCode);
    assertEquals("Locale-specific plus sign should be used", u"1.23b∔", result);
    // Test round-trip with parse
    expect2(df, -456, u"456.00a∸");
    expect2(df, 456, u"456.00b∔");
}