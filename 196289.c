void NumberFormatTest::Test11645_ApplyPatternEquality() {
    IcuTestErrorCode status(*this, "Test11645_ApplyPatternEquality");
    const char16_t* pattern = u"#,##0.0#";
    LocalPointer<DecimalFormat> fmt((DecimalFormat*) NumberFormat::createInstance(status), status);
    if (!assertSuccess("", status, true, __FILE__, __LINE__)) { return; }
    fmt->applyPattern(pattern, status);
    LocalPointer<DecimalFormat> fmtCopy;

    static const int32_t newMultiplier = 37;
    fmtCopy.adoptInstead(new DecimalFormat(*fmt));
    assertFalse("Value before setter", fmtCopy->getMultiplier() == newMultiplier);
    fmtCopy->setMultiplier(newMultiplier);
    assertEquals("Value after setter", fmtCopy->getMultiplier(), newMultiplier);
    fmtCopy->applyPattern(pattern, status);
    assertEquals("Value after applyPattern", fmtCopy->getMultiplier(), newMultiplier);
    assertFalse("multiplier", *fmt == *fmtCopy);

    static const NumberFormat::ERoundingMode newRoundingMode = NumberFormat::ERoundingMode::kRoundCeiling;
    fmtCopy.adoptInstead(new DecimalFormat(*fmt));
    assertFalse("Value before setter", fmtCopy->getRoundingMode() == newRoundingMode);
    fmtCopy->setRoundingMode(newRoundingMode);
    assertEquals("Value after setter", fmtCopy->getRoundingMode(), newRoundingMode);
    fmtCopy->applyPattern(pattern, status);
    assertEquals("Value after applyPattern", fmtCopy->getRoundingMode(), newRoundingMode);
    assertFalse("roundingMode", *fmt == *fmtCopy);

    static const char16_t *const newCurrency = u"EAT";
    fmtCopy.adoptInstead(new DecimalFormat(*fmt));
    assertFalse("Value before setter", fmtCopy->getCurrency() == newCurrency);
    fmtCopy->setCurrency(newCurrency);
    assertEquals("Value after setter", fmtCopy->getCurrency(), newCurrency);
    fmtCopy->applyPattern(pattern, status);
    assertEquals("Value after applyPattern", fmtCopy->getCurrency(), newCurrency);
    assertFalse("currency", *fmt == *fmtCopy);

    static const UCurrencyUsage newCurrencyUsage = UCurrencyUsage::UCURR_USAGE_CASH;
    fmtCopy.adoptInstead(new DecimalFormat(*fmt));
    assertFalse("Value before setter", fmtCopy->getCurrencyUsage() == newCurrencyUsage);
    fmtCopy->setCurrencyUsage(newCurrencyUsage, status);
    assertEquals("Value after setter", fmtCopy->getCurrencyUsage(), newCurrencyUsage);
    fmtCopy->applyPattern(pattern, status);
    assertEquals("Value after applyPattern", fmtCopy->getCurrencyUsage(), newCurrencyUsage);
    assertFalse("currencyUsage", *fmt == *fmtCopy);
}