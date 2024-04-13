void NumberFormatTest::Test11640_TripleCurrencySymbol() {
    IcuTestErrorCode status(*this, "Test11640_TripleCurrencySymbol");
    UnicodeString actual;
    DecimalFormat dFormat(u"¤¤¤ 0", status);
    if (U_FAILURE(status)) {
        dataerrln("Failure creating DecimalFormat %s", u_errorName(status));
        return;
    }
    dFormat.setCurrency(u"USD");
    UnicodeString result;
    dFormat.getPositivePrefix(result);
    assertEquals("Triple-currency should give long name on getPositivePrefix",
                "US dollars ", result);
}