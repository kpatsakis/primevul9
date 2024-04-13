void NumberFormatTest::Test11897_LocalizedPatternSeparator() {
    IcuTestErrorCode status(*this, "Test11897_LocalizedPatternSeparator");

    // In a locale with a different <list> symbol, like arabic,
    // kPatternSeparatorSymbol should still be ';'
    {
        DecimalFormatSymbols dfs("ar", status);
        assertEquals("pattern separator symbol should be ;",
                u";",
                dfs.getSymbol(DecimalFormatSymbols::kPatternSeparatorSymbol));
    }

    // However, the custom symbol should be used in localized notation
    // when set manually via API
    {
        DecimalFormatSymbols dfs("en", status);
        dfs.setSymbol(DecimalFormatSymbols::kPatternSeparatorSymbol, u"!", FALSE);
        DecimalFormat df(u"0", dfs, status);
        if (!assertSuccess("", status, true, __FILE__, __LINE__)) { return; }
        df.applyPattern("a0;b0", status); // should not throw
        UnicodeString result;
        assertEquals("should apply the normal pattern",
                df.getNegativePrefix(result.remove()),
                "b");
        df.applyLocalizedPattern(u"c0!d0", status); // should not throw
        assertEquals("should apply the localized pattern",
                df.getNegativePrefix(result.remove()),
                "d");
    }
}