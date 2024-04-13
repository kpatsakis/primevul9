NumberFormatTest::TestQuotes(void)
{
    UErrorCode status = U_ZERO_ERROR;
    UnicodeString *pat;
    DecimalFormatSymbols *sym = new DecimalFormatSymbols(Locale::getUS(), status);
    if (U_FAILURE(status)) {
        errcheckln(status, "Fail to create DecimalFormatSymbols - %s", u_errorName(status));
        delete sym;
        return;
    }
    pat = new UnicodeString("a'fo''o'b#");
    DecimalFormat *fmt = new DecimalFormat(*pat, *sym, status);
    UnicodeString s;
    ((NumberFormat*)fmt)->format((int32_t)123, s);
    logln((UnicodeString)"Pattern \"" + *pat + "\"");
    logln((UnicodeString)" Format 123 -> " + escape(s));
    if (!(s=="afo'ob123"))
        errln((UnicodeString)"FAIL: Expected afo'ob123");

    s.truncate(0);
    delete fmt;
    delete pat;

    pat = new UnicodeString("a''b#");
    fmt = new DecimalFormat(*pat, *sym, status);
    ((NumberFormat*)fmt)->format((int32_t)123, s);
    logln((UnicodeString)"Pattern \"" + *pat + "\"");
    logln((UnicodeString)" Format 123 -> " + escape(s));
    if (!(s=="a'b123"))
        errln((UnicodeString)"FAIL: Expected a'b123");
    delete fmt;
    delete pat;
    delete sym;
}