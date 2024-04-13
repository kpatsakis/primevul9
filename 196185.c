void NumberFormatTest::Test11739_ParseLongCurrency() {
    IcuTestErrorCode status(*this, "Test11739_ParseLongCurrency");
    LocalPointer<NumberFormat> nf(NumberFormat::createCurrencyInstance("sr_BA", status));
    if (status.errDataIfFailureAndReset()) { return; }
    ((DecimalFormat*) nf.getAlias())->applyPattern(u"#,##0.0 ¤¤¤", status);
    ParsePosition ppos(0);
    LocalPointer<CurrencyAmount> result(nf->parseCurrency(u"1.500 амерички долар", ppos));
    assertEquals("Should parse to 1500 USD", -1, ppos.getErrorIndex());
    assertEquals("Should parse to 1500 USD", 1500LL, result->getNumber().getInt64(status));
    assertEquals("Should parse to 1500 USD", u"USD", result->getISOCurrency());
}