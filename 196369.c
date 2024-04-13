void NumberFormatTest::TestPerMill() {
    UErrorCode ec = U_ZERO_ERROR;
    UnicodeString str;
    DecimalFormat fmt(ctou("###.###\\u2030"), ec);
    if (!assertSuccess("DecimalFormat ct", ec)) return;
    assertEquals("0.4857 x ###.###\\u2030",
                 ctou("485.7\\u2030"), fmt.format(0.4857, str), true);

    DecimalFormatSymbols sym(Locale::getUS(), ec);
    if (!assertSuccess("", ec, true, __FILE__, __LINE__)) {
        return;
    }
    sym.setSymbol(DecimalFormatSymbols::kPerMillSymbol, ctou("m"));
    DecimalFormat fmt2("", sym, ec);
    if (!assertSuccess("", ec, true, __FILE__, __LINE__)) {
        return;
    }
    fmt2.applyLocalizedPattern("###.###m", ec);
    if (!assertSuccess("setup", ec)) return;
    str.truncate(0);
    assertEquals("0.4857 x ###.###m",
                 "485.7m", fmt2.format(0.4857, str));
}