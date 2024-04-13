void NumberFormatTest::TestFormatCurrencyPlural() {
    UErrorCode status = U_ZERO_ERROR;
    Locale locale = Locale::createCanonical("en_US");
    NumberFormat *fmt = NumberFormat::createInstance(locale, UNUM_CURRENCY_PLURAL, status);
    if (U_FAILURE(status)) {
        dataerrln("Error creating NumberFormat - %s", u_errorName(status));
        return;
    }
   UnicodeString formattedNum;
   fmt->format(11234.567, formattedNum, NULL, status);
   assertEquals("", "11,234.57 US dollars", formattedNum);
   delete fmt;
}