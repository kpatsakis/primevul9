void NumberFormatTest::expectCurrency(NumberFormat& nf, const Locale& locale,
                                      double value, const UnicodeString& string) {
    UErrorCode ec = U_ZERO_ERROR;
    DecimalFormat& fmt = * (DecimalFormat*) &nf;
    const UChar DEFAULT_CURR[] = {45/*-*/,0};
    UChar curr[4];
    u_strcpy(curr, DEFAULT_CURR);
    if (*locale.getLanguage() != 0) {
        ucurr_forLocale(locale.getName(), curr, 4, &ec);
        assertSuccess("ucurr_forLocale", ec);
        fmt.setCurrency(curr, ec);
        assertSuccess("DecimalFormat::setCurrency", ec);
        fmt.setCurrency(curr); //Deprecated variant, for coverage only
    }
    UnicodeString s;
    fmt.format(value, s);
    s.findAndReplace((UChar32)0x00A0, (UChar32)0x0020);

    // Default display of the number yields "1234.5599999999999"
    // instead of "1234.56".  Use a formatter to fix this.
    NumberFormat* f =
        NumberFormat::createInstance(Locale::getUS(), ec);
    UnicodeString v;
    if (U_FAILURE(ec)) {
        // Oops; bad formatter.  Use default op+= display.
        v = (UnicodeString)"" + value;
    } else {
        f->setMaximumFractionDigits(4);
        f->setGroupingUsed(FALSE);
        f->format(value, v);
    }
    delete f;

    if (s == string) {
        logln((UnicodeString)"Ok: " + v + " x " + curr + " => " + prettify(s));
    } else {
        errln((UnicodeString)"FAIL: " + v + " x " + curr + " => " + prettify(s) +
              ", expected " + prettify(string));
    }
}