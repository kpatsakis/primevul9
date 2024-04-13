void NumberFormatTest::expectPad(DecimalFormat& fmt, const UnicodeString& pat,
                                 int32_t pos, int32_t width, const UnicodeString& pad) {
    int32_t apos = 0, awidth = 0;
    UnicodeString apadStr;
    UErrorCode status = U_ZERO_ERROR;
    fmt.applyPattern(pat, status);
    if (U_SUCCESS(status)) {
        apos = fmt.getPadPosition();
        awidth = fmt.getFormatWidth();
        apadStr=fmt.getPadCharacterString();
    } else {
        apos = -1;
        awidth = width;
        apadStr = pad;
    }
    if (apos == pos && awidth == width && apadStr == pad) {
        UnicodeString infoStr;
        if (pos == ILLEGAL) {
            infoStr = UnicodeString(" width=", "") + awidth + UnicodeString(" pad=", "") + apadStr;
        }
        logln(UnicodeString("Ok   \"") + pat + "\" pos=" + apos + infoStr);
    } else {
        errln(UnicodeString("FAIL \"") + pat + "\" pos=" + apos +
              " width=" + awidth + " pad=" + apadStr +
              ", expected " + pos + " " + width + " " + pad);
    }
}