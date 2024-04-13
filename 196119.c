NumberFormatTest::TestCurrencySign(void)
{
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols* sym = new DecimalFormatSymbols(Locale::getUS(), status);
    UnicodeString pat;
    UChar currency = 0x00A4;
    if (U_FAILURE(status)) {
        errcheckln(status, "Fail to create DecimalFormatSymbols - %s", u_errorName(status));
        delete sym;
        return;
    }
    // "\xA4#,##0.00;-\xA4#,##0.00"
    pat.append(currency).append("#,##0.00;-").
        append(currency).append("#,##0.00");
    DecimalFormat *fmt = new DecimalFormat(pat, *sym, status);
    UnicodeString s; ((NumberFormat*)fmt)->format(1234.56, s);
    pat.truncate(0);
    logln((UnicodeString)"Pattern \"" + fmt->toPattern(pat) + "\"");
    logln((UnicodeString)" Format " + 1234.56 + " -> " + escape(s));
    if (s != "$1,234.56") dataerrln((UnicodeString)"FAIL: Expected $1,234.56");
    s.truncate(0);
    ((NumberFormat*)fmt)->format(- 1234.56, s);
    logln((UnicodeString)" Format " + (-1234.56) + " -> " + escape(s));
    if (s != "-$1,234.56") dataerrln((UnicodeString)"FAIL: Expected -$1,234.56");
    delete fmt;
    pat.truncate(0);
    // "\xA4\xA4 #,##0.00;\xA4\xA4 -#,##0.00"
    pat.append(currency).append(currency).
        append(" #,##0.00;").
        append(currency).append(currency).
        append(" -#,##0.00");
    fmt = new DecimalFormat(pat, *sym, status);
    s.truncate(0);
    ((NumberFormat*)fmt)->format(1234.56, s);
    logln((UnicodeString)"Pattern \"" + fmt->toPattern(pat) + "\"");
    logln((UnicodeString)" Format " + 1234.56 + " -> " + escape(s));
    if (s != "USD 1,234.56") dataerrln((UnicodeString)"FAIL: Expected USD 1,234.56");
    s.truncate(0);
    ((NumberFormat*)fmt)->format(-1234.56, s);
    logln((UnicodeString)" Format " + (-1234.56) + " -> " + escape(s));
    if (s != "USD -1,234.56") dataerrln((UnicodeString)"FAIL: Expected USD -1,234.56");
    delete fmt;
    delete sym;
    if (U_FAILURE(status)) errln((UnicodeString)"FAIL: Status " + u_errorName(status));
}