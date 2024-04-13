void NumberFormatTest::Test11735_ExceptionIssue() {
    IcuTestErrorCode status(*this, "Test11735_ExceptionIssue");
    Locale enLocale("en");
    DecimalFormatSymbols symbols(enLocale, status);
    if (status.isSuccess()) {
        DecimalFormat fmt("0", symbols, status);
        assertSuccess("Fail: Construct DecimalFormat formatter", status, true, __FILE__, __LINE__);
        ParsePosition ppos(0);
        fmt.parseCurrency("53.45", ppos);  // NPE thrown here in ICU4J.
        assertEquals("Issue11735 ppos", 0, ppos.getIndex());
    }
}