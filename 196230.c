void NumberFormatTest::TestLocalizedPatternSymbolCoverage() {
    IcuTestErrorCode errorCode(*this, "TestLocalizedPatternSymbolCoverage");
    // Ticket #12961: DecimalFormat::toLocalizedPattern() is not working as designed.
    DecimalFormatSymbols dfs(errorCode);
    dfs.setSymbol(DecimalFormatSymbols::kGroupingSeparatorSymbol, u'⁖');
    dfs.setSymbol(DecimalFormatSymbols::kDecimalSeparatorSymbol, u'⁘');
    dfs.setSymbol(DecimalFormatSymbols::kPatternSeparatorSymbol, u'⁙');
    dfs.setSymbol(DecimalFormatSymbols::kDigitSymbol, u'▰');
    dfs.setSymbol(DecimalFormatSymbols::kZeroDigitSymbol, u'໐');
    dfs.setSymbol(DecimalFormatSymbols::kSignificantDigitSymbol, u'⁕');
    dfs.setSymbol(DecimalFormatSymbols::kPlusSignSymbol, u'†');
    dfs.setSymbol(DecimalFormatSymbols::kMinusSignSymbol, u'‡');
    dfs.setSymbol(DecimalFormatSymbols::kPercentSymbol, u'⁜');
    dfs.setSymbol(DecimalFormatSymbols::kPerMillSymbol, u'‱');
    dfs.setSymbol(DecimalFormatSymbols::kExponentialSymbol, u"⁑⁑"); // tests multi-char sequence
    dfs.setSymbol(DecimalFormatSymbols::kPadEscapeSymbol, u'⁂');

    {
        UnicodeString standardPattern(u"#,##0.05+%;#,##0.05-%");
        UnicodeString localizedPattern(u"▰⁖▰▰໐⁘໐໕†⁜⁙▰⁖▰▰໐⁘໐໕‡⁜");

        DecimalFormat df1("#", new DecimalFormatSymbols(dfs), errorCode);
        df1.applyPattern(standardPattern, errorCode);
        DecimalFormat df2("#", new DecimalFormatSymbols(dfs), errorCode);
        df2.applyLocalizedPattern(localizedPattern, errorCode);
        assertTrue("DecimalFormat instances should be equal", df1 == df2);
        UnicodeString p2;
        assertEquals("toPattern should match on localizedPattern instance",
                standardPattern, df2.toPattern(p2));
        UnicodeString lp1;
        assertEquals("toLocalizedPattern should match on standardPattern instance",
                localizedPattern, df1.toLocalizedPattern(lp1));
    }

    {
        UnicodeString standardPattern(u"* @@@E0‰");
        UnicodeString localizedPattern(u"⁂ ⁕⁕⁕⁑⁑໐‱");

        DecimalFormat df1("#", new DecimalFormatSymbols(dfs), errorCode);
        df1.applyPattern(standardPattern, errorCode);
        DecimalFormat df2("#", new DecimalFormatSymbols(dfs), errorCode);
        df2.applyLocalizedPattern(localizedPattern, errorCode);
        assertTrue("DecimalFormat instances should be equal", df1 == df2);
        UnicodeString p2;
        assertEquals("toPattern should match on localizedPattern instance",
                standardPattern, df2.toPattern(p2));
        UnicodeString lp1;
        assertEquals("toLocalizedPattern should match on standardPattern instance",
                localizedPattern, df1.toLocalizedPattern(lp1));
    }
}