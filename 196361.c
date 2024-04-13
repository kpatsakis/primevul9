void NumberFormatTest::TestSurrogateSupport(void) {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols custom(Locale::getUS(), status);
    CHECK(status, "DecimalFormatSymbols constructor");

    custom.setSymbol(DecimalFormatSymbols::kDecimalSeparatorSymbol, "decimal");
    custom.setSymbol(DecimalFormatSymbols::kPlusSignSymbol, "plus");
    custom.setSymbol(DecimalFormatSymbols::kMinusSignSymbol, " minus ");
    custom.setSymbol(DecimalFormatSymbols::kExponentialSymbol, "exponent");

    UnicodeString patternStr("*\\U00010000##.##", "");
    patternStr = patternStr.unescape();
    UnicodeString expStr("\\U00010000\\U00010000\\U00010000\\U000100000", "");
    expStr = expStr.unescape();
    expect2(new DecimalFormat(patternStr, custom, status),
           int32_t(0), expStr, status);

    status = U_ZERO_ERROR;
    expect2(new DecimalFormat("*^##.##", custom, status),
           int32_t(0), "^^^^0", status);
    status = U_ZERO_ERROR;
    expect2(new DecimalFormat("##.##", custom, status),
           -1.3, " minus 1decimal3", status);
    status = U_ZERO_ERROR;
    expect2(new DecimalFormat("##0.0####E0 'g-m/s^2'", custom, status),
           int32_t(0), "0decimal0exponent0 g-m/s^2", status);
    status = U_ZERO_ERROR;
    expect(new DecimalFormat("##0.0####E0 'g-m/s^2'", custom, status),
           1.0/3, "333decimal333exponent minus 3 g-m/s^2", status);
    status = U_ZERO_ERROR;
    expect2(new DecimalFormat("##0.0#### 'g-m/s^2'", custom, status),
           int32_t(0), "0decimal0 g-m/s^2", status);
    status = U_ZERO_ERROR;
    expect(new DecimalFormat("##0.0#### 'g-m/s^2'", custom, status),
           1.0/3, "0decimal33333 g-m/s^2", status);

    UnicodeString zero((UChar32)0x10000);
    UnicodeString one((UChar32)0x10001);
    UnicodeString two((UChar32)0x10002);
    UnicodeString five((UChar32)0x10005);
    custom.setSymbol(DecimalFormatSymbols::kZeroDigitSymbol, zero);
    custom.setSymbol(DecimalFormatSymbols::kOneDigitSymbol, one);
    custom.setSymbol(DecimalFormatSymbols::kTwoDigitSymbol, two);
    custom.setSymbol(DecimalFormatSymbols::kFiveDigitSymbol, five);
    expStr = UnicodeString("\\U00010001decimal\\U00010002\\U00010005\\U00010000", "");
    expStr = expStr.unescape();
    status = U_ZERO_ERROR;
    expect2(new DecimalFormat("##0.000", custom, status),
           1.25, expStr, status);

    custom.setSymbol(DecimalFormatSymbols::kZeroDigitSymbol, (UChar)0x30);
    custom.setSymbol(DecimalFormatSymbols::kCurrencySymbol, "units of money");
    custom.setSymbol(DecimalFormatSymbols::kMonetarySeparatorSymbol, "money separator");
    patternStr = UNICODE_STRING_SIMPLE("0.00 \\u00A4' in your bank account'");
    patternStr = patternStr.unescape();
    expStr = UnicodeString(" minus 20money separator00 units of money in your bank account", "");
    status = U_ZERO_ERROR;
    expect2(new DecimalFormat(patternStr, custom, status),
           int32_t(-20), expStr, status);

    custom.setSymbol(DecimalFormatSymbols::kPercentSymbol, "percent");
    patternStr = "'You''ve lost ' -0.00 %' of your money today'";
    patternStr = patternStr.unescape();
    expStr = UnicodeString(" minus You've lost   minus 2000decimal00 percent of your money today", "");
    status = U_ZERO_ERROR;
    expect2(new DecimalFormat(patternStr, custom, status),
           int32_t(-20), expStr, status);
}