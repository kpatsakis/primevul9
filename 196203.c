void NumberFormatTest::Test11626_CustomizeCurrencyPluralInfo() {
    IcuTestErrorCode errorCode(*this, "Test11626_CustomizeCurrencyPluralInfo");
    // Ticket #11626: No unit test demonstrating how to use CurrencyPluralInfo to
    // change formatting spelled out currencies
    // Use locale sr because it has interesting plural rules.
    Locale locale("sr");
    LocalPointer<DecimalFormatSymbols> symbols(new DecimalFormatSymbols(locale, errorCode), errorCode);
    CurrencyPluralInfo info(locale, errorCode);
    if (!assertSuccess("", errorCode, true, __FILE__, __LINE__)) { return; }
    info.setCurrencyPluralPattern(u"one", u"0 qwerty", errorCode);
    info.setCurrencyPluralPattern(u"few", u"0 dvorak", errorCode);
    DecimalFormat df(u"#", symbols.orphan(), UNUM_CURRENCY_PLURAL, errorCode);
    df.setCurrencyPluralInfo(info);
    df.setCurrency(u"USD");
    df.setMaximumFractionDigits(0);

    UnicodeString result;
    assertEquals("Plural one", u"1 qwerty", df.format(1, result, errorCode));
    assertEquals("Plural few", u"3 dvorak", df.format(3, result.remove(), errorCode));
    assertEquals("Plural other", u"99 америчких долара", df.format(99, result.remove(), errorCode));

    info.setPluralRules(u"few: n is 1; one: n in 2..4", errorCode);
    df.setCurrencyPluralInfo(info);
    assertEquals("Plural one", u"1 dvorak", df.format(1, result.remove(), errorCode));
    assertEquals("Plural few", u"3 qwerty", df.format(3, result.remove(), errorCode));
    assertEquals("Plural other", u"99 америчких долара", df.format(99, result.remove(), errorCode));
}