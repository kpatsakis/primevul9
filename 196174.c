void NumberFormatTest::Test11035_FormatCurrencyAmount() {
    UErrorCode status = U_ZERO_ERROR;
    double amount = 12345.67;
    const char16_t* expected = u"12,345$67 ​";

    // Test two ways to set a currency via API

    Locale loc1 = Locale("pt_PT");
    LocalPointer<NumberFormat> fmt1(NumberFormat::createCurrencyInstance("loc1", status),
                                    status);
    if (U_FAILURE(status)) {
      dataerrln("%s %d NumberFormat instance fmt1 is null",  __FILE__, __LINE__);
      return;
    }
    fmt1->setCurrency(u"PTE", status);
    assertSuccess("Setting currency on fmt1", status);
    UnicodeString actualSetCurrency;
    fmt1->format(amount, actualSetCurrency);

    Locale loc2 = Locale("pt_PT@currency=PTE");
    LocalPointer<NumberFormat> fmt2(NumberFormat::createCurrencyInstance(loc2, status));
    assertSuccess("Creating fmt2", status);
    UnicodeString actualLocaleString;
    fmt2->format(amount, actualLocaleString);

    // TODO: The following test will fail until DecimalFormat wraps NumberFormatter.
    if (!logKnownIssue("13574")) {
        assertEquals("Custom Currency Pattern, Set Currency", expected, actualSetCurrency);
    }
}