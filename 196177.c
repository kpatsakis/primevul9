void NumberFormatTest::Test13804_EmptyStringsWhenParsing() {
    IcuTestErrorCode status(*this, "Test13804_EmptyStringsWhenParsing");

    DecimalFormatSymbols dfs("en", status);
    if (status.errIfFailureAndReset()) {
        return;
    }
    dfs.setSymbol(DecimalFormatSymbols::kCurrencySymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kDecimalSeparatorSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kZeroDigitSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kOneDigitSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kTwoDigitSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kThreeDigitSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kFourDigitSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kFiveDigitSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kSixDigitSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kSevenDigitSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kEightDigitSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kNineDigitSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kExponentMultiplicationSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kExponentialSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kGroupingSeparatorSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kInfinitySymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kIntlCurrencySymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kMinusSignSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kMonetarySeparatorSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kMonetaryGroupingSeparatorSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kNaNSymbol, u"", FALSE);
    dfs.setPatternForCurrencySpacing(UNUM_CURRENCY_INSERT, FALSE, u"");
    dfs.setPatternForCurrencySpacing(UNUM_CURRENCY_INSERT, TRUE, u"");
    dfs.setSymbol(DecimalFormatSymbols::kPercentSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kPerMillSymbol, u"", FALSE);
    dfs.setSymbol(DecimalFormatSymbols::kPlusSignSymbol, u"", FALSE);

    DecimalFormat df("0", dfs, status);
    if (status.errIfFailureAndReset()) {
        return;
    }
    df.setGroupingUsed(TRUE);
    df.setScientificNotation(TRUE);
    df.setLenient(TRUE); // enable all matchers
    {
        UnicodeString result;
        df.format(0, result); // should not crash or hit infinite loop
    }
    const char16_t* samples[] = {
            u"",
            u"123",
            u"$123",
            u"-",
            u"+",
            u"44%",
            u"1E+2.3"
    };
    for (auto& sample : samples) {
        logln(UnicodeString(u"Attempting parse on: ") + sample);
        status.setScope(sample);
        // We don't care about the results, only that we don't crash and don't loop.
        Formattable result;
        ParsePosition ppos(0);
        df.parse(sample, result, ppos);
        ppos = ParsePosition(0);
        LocalPointer<CurrencyAmount> curramt(df.parseCurrency(sample, ppos));
        status.errIfFailureAndReset();
    }

    // Test with a nonempty exponent separator symbol to cover more code
    dfs.setSymbol(DecimalFormatSymbols::kExponentialSymbol, u"E", FALSE);
    df.setDecimalFormatSymbols(dfs);
    {
        Formattable result;
        ParsePosition ppos(0);
        df.parse(u"1E+2.3", result, ppos);
    }
}