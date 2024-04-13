void NumberFormatTest::expectPad(DecimalFormat& fmt, const UnicodeString& pat,
                                 int32_t pos, int32_t width, UChar pad) {
    expectPad(fmt, pat, pos, width, UnicodeString(pad));
}