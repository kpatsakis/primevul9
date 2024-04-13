NumberFormatTest::TestScientificGrouping() {
    // jb 2552
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormat fmt("##0.00E0",status);
    if (assertSuccess("", status, true, __FILE__, __LINE__)) {
        expect(fmt, .01234, "12.3E-3");
        expect(fmt, .1234, "123E-3");
        expect(fmt, 1.234, "1.23E0");
        expect(fmt, 12.34, "12.3E0");
        expect(fmt, 123.4, "123E0");
        expect(fmt, 1234., "1.23E3");
    }
}