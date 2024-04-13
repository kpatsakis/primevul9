void NumberFormatTest::expect3(NumberFormat& fmt, const Formattable& n, const UnicodeString& str) {
    // Don't round-trip format test, since we explicitly do it
    expect_rbnf(fmt, n, str, FALSE);
    expect_rbnf(fmt, str, n);
}