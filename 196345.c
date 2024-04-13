void NumberFormatTest::expectParseCurrency(const NumberFormat &fmt, const UChar* currency, double amount, const char *text) {
    ParsePosition ppos;
    UnicodeString utext = ctou(text);
    LocalPointer<CurrencyAmount> currencyAmount(fmt.parseCurrency(utext, ppos));
    if (!ppos.getIndex()) {
        errln(UnicodeString("Parse of ") + utext + " should have succeeded.");
        return;
    }
    UErrorCode status = U_ZERO_ERROR;

    char theInfo[100];
    sprintf(theInfo, "For locale %s, string \"%s\", currency ",
            fmt.getLocale(ULOC_ACTUAL_LOCALE, status).getBaseName(),
            text);
    u_austrcpy(theInfo+uprv_strlen(theInfo), currency);

    char theOperation[100];

    uprv_strcpy(theOperation, theInfo);
    uprv_strcat(theOperation, ", check amount:");
    assertTrue(theOperation, amount ==  currencyAmount->getNumber().getDouble(status));

    uprv_strcpy(theOperation, theInfo);
    uprv_strcat(theOperation, ", check currency:");
    assertEquals(theOperation, currency, currencyAmount->getISOCurrency());
}