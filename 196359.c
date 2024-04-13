void NumberFormatTest::TestAccountingCurrency() {
    UErrorCode status = U_ZERO_ERROR;
    UNumberFormatStyle style = UNUM_CURRENCY_ACCOUNTING;

    expect(NumberFormat::createInstance("en_US", style, status),
        (Formattable)(double)1234.5, "$1,234.50", TRUE, status);
    expect(NumberFormat::createInstance("en_US", style, status),
        (Formattable)(double)-1234.5, "($1,234.50)", TRUE, status);
    expect(NumberFormat::createInstance("en_US", style, status),
        (Formattable)(double)0, "$0.00", TRUE, status);
    expect(NumberFormat::createInstance("en_US", style, status),
        (Formattable)(double)-0.2, "($0.20)", TRUE, status);
    expect(NumberFormat::createInstance("ja_JP", style, status),
        (Formattable)(double)10000, UnicodeString("\\uFFE510,000").unescape(), TRUE, status);
    expect(NumberFormat::createInstance("ja_JP", style, status),
        (Formattable)(double)-1000.5, UnicodeString("(\\uFFE51,000)").unescape(), FALSE, status);
    expect(NumberFormat::createInstance("de_DE", style, status),
        (Formattable)(double)-23456.7, UnicodeString("-23.456,70\\u00A0\\u20AC").unescape(), TRUE, status);
}