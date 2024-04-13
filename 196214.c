void NumberFormatTest::Test13148_ParseGroupingSeparators() {
  IcuTestErrorCode status(*this, "Test13148");
  LocalPointer<DecimalFormat> fmt(
      (DecimalFormat*)NumberFormat::createInstance("en-ZA", status), status);
  if (!assertSuccess("", status, true, __FILE__, __LINE__)) { return; }

  DecimalFormatSymbols symbols = *fmt->getDecimalFormatSymbols();

  symbols.setSymbol(DecimalFormatSymbols::kDecimalSeparatorSymbol, u'.');
  symbols.setSymbol(DecimalFormatSymbols::kGroupingSeparatorSymbol, u',');
  fmt->setDecimalFormatSymbols(symbols);
  Formattable number;
  fmt->parse(u"300,000", number, status);
  assertEquals("Should parse as 300000", 300000LL, number.getInt64(status));
}