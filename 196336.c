void NumberFormatTest::Test11318_DoubleConversion() {
    IcuTestErrorCode status(*this, "Test11318_DoubleConversion");
    LocalPointer<NumberFormat> nf(NumberFormat::createInstance("en", status), status);
    if (U_FAILURE(status)) {
      dataerrln("%s %d Error in NumberFormat instance creation",  __FILE__, __LINE__);
      return;
    }
    nf->setMaximumFractionDigits(40);
    nf->setMaximumIntegerDigits(40);
    UnicodeString appendTo;
    nf->format(999999999999999.9, appendTo);
    assertEquals("Should render all digits", u"999,999,999,999,999.9", appendTo);
}