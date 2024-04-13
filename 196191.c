NumberFormatTest::TestParse(void)
{
    UErrorCode status = U_ZERO_ERROR;
    UnicodeString arg("0");
    DecimalFormat* format = new DecimalFormat("00", status);
    //try {
        Formattable n; format->parse(arg, n, status);
        logln((UnicodeString)"parse(" + arg + ") = " + n.getLong());
        if (n.getType() != Formattable::kLong ||
            n.getLong() != 0) errln((UnicodeString)"FAIL: Expected 0");
    delete format;
    if (U_FAILURE(status)) errcheckln(status, (UnicodeString)"FAIL: Status " + u_errorName(status));
    //}
    //catch(Exception e) {
    //    errln((UnicodeString)"Exception caught: " + e);
    //}
}