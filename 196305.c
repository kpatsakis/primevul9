void NumberFormatTest::TestZeroScientific10547() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormat fmt("0.00E0", status);
    if (!assertSuccess("Format creation", status)) {
        return;
    }
    UnicodeString out;
    fmt.format(-0.0, out);
    assertEquals("format", "-0.00E0", out, true);
}