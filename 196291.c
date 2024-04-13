void NumberFormatTest::expectPat(DecimalFormat& fmt, const UnicodeString& exp) {
    UnicodeString pat;
    fmt.toPattern(pat);
    if (pat == exp) {
        logln(UnicodeString("Ok   \"") + pat + "\"");
    } else {
        errln(UnicodeString("FAIL \"") + pat + "\", expected \"" + exp + "\"");
    }
}