NumberFormatTest::TestAPI(void)
{
  logln("Test API");
  UErrorCode status = U_ZERO_ERROR;
  NumberFormat *test = NumberFormat::createInstance("root", status);
  if(U_FAILURE(status)) {
    dataerrln("unable to create format object - %s", u_errorName(status));
  }
  if(test != NULL) {
    test->setMinimumIntegerDigits(10);
    test->setMaximumIntegerDigits(1);

    test->setMinimumFractionDigits(10);
    test->setMaximumFractionDigits(1);

    UnicodeString result;
    FieldPosition pos;
    Formattable bla("Paja Patak"); // Donald Duck for non Serbian speakers
    test->format(bla, result, pos, status);
    if(U_SUCCESS(status)) {
      errln("Yuck... Formatted a duck... As a number!");
    } else {
      status = U_ZERO_ERROR;
    }

    result.remove();
    int64_t ll = 12;
    test->format(ll, result);
    assertEquals("format int64_t error", u"2.0", result);

    test->setMinimumIntegerDigits(4);
    test->setMinimumFractionDigits(4);

    result.remove();
    test->format(ll, result);
    assertEquals("format int64_t error", u"0,012.0000", result);

    ParsePosition ppos;
    LocalPointer<CurrencyAmount> currAmt(test->parseCurrency("",ppos));
    // old test for (U_FAILURE(status)) was bogus here, method does not set status!
    if (ppos.getIndex()) {
        errln("Parsed empty string as currency");
    }

    delete test;
  }
}