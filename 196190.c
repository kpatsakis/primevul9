void NumberFormatTest::Test13055_PercentageRounding() {
  IcuTestErrorCode status(*this, "PercentageRounding");
  UnicodeString actual;
  LocalPointer<NumberFormat>pFormat(NumberFormat::createPercentInstance("en_US", status));
  if (U_FAILURE(status)) {
      dataerrln("Failure creating DecimalFormat %s", u_errorName(status));
      return;
  }
  pFormat->setMaximumFractionDigits(0);
  pFormat->setRoundingMode(DecimalFormat::kRoundHalfEven);
  pFormat->format(2.155, actual);
  assertEquals("Should round percent toward even number", "216%", actual);
}