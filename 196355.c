NumberFormatTest::TestInt64() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormat fmt("#.#E0",status);
    if (U_FAILURE(status)) {
        dataerrln("Error creating DecimalFormat - %s", u_errorName(status));
        return;
    }
    fmt.setMaximumFractionDigits(20);
    if (U_SUCCESS(status)) {
        expect(fmt, (Formattable)(int64_t)0, "0E0");
        expect(fmt, (Formattable)(int64_t)-1, "-1E0");
        expect(fmt, (Formattable)(int64_t)1, "1E0");
        expect(fmt, (Formattable)(int64_t)2147483647, "2.147483647E9");
        expect(fmt, (Formattable)((int64_t)-2147483647-1), "-2.147483648E9");
        expect(fmt, (Formattable)(int64_t)U_INT64_MAX, "9.223372036854775807E18");
        expect(fmt, (Formattable)(int64_t)U_INT64_MIN, "-9.223372036854775808E18");
    }

    // also test digitlist
/*    int64_t int64max = U_INT64_MAX;
    int64_t int64min = U_INT64_MIN;
    const char* int64maxstr = "9223372036854775807";
    const char* int64minstr = "-9223372036854775808";
    UnicodeString fail("fail: ");

    // test max int64 value
    DigitList dl;
    setFromString(dl, int64maxstr);
    {
        if (!dl.fitsIntoInt64(FALSE)) {
            errln(fail + int64maxstr + " didn't fit");
        }
        int64_t int64Value = dl.getInt64();
        if (int64Value != int64max) {
            errln(fail + int64maxstr);
        }
        dl.set(int64Value);
        int64Value = dl.getInt64();
        if (int64Value != int64max) {
            errln(fail + int64maxstr);
        }
    }
    // test negative of max int64 value (1 shy of min int64 value)
    dl.fIsPositive = FALSE;
    {
        if (!dl.fitsIntoInt64(FALSE)) {
            errln(fail + "-" + int64maxstr + " didn't fit");
        }
        int64_t int64Value = dl.getInt64();
        if (int64Value != -int64max) {
            errln(fail + "-" + int64maxstr);
        }
        dl.set(int64Value);
        int64Value = dl.getInt64();
        if (int64Value != -int64max) {
            errln(fail + "-" + int64maxstr);
        }
    }
    // test min int64 value
    setFromString(dl, int64minstr);
    {
        if (!dl.fitsIntoInt64(FALSE)) {
            errln(fail + "-" + int64minstr + " didn't fit");
        }
        int64_t int64Value = dl.getInt64();
        if (int64Value != int64min) {
            errln(fail + int64minstr);
        }
        dl.set(int64Value);
        int64Value = dl.getInt64();
        if (int64Value != int64min) {
            errln(fail + int64minstr);
        }
    }
    // test negative of min int 64 value (1 more than max int64 value)
    dl.fIsPositive = TRUE; // won't fit
    {
        if (dl.fitsIntoInt64(FALSE)) {
            errln(fail + "-(" + int64minstr + ") didn't fit");
        }
    }*/
}