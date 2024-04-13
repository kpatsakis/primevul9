void NumberFormatTest::TestCustomCurrencySignAndSeparator() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols custom(Locale::getUS(), status);
    CHECK(status, "DecimalFormatSymbols constructor");

    custom.setSymbol(DecimalFormatSymbols::kCurrencySymbol, "*");
    custom.setSymbol(DecimalFormatSymbols::kMonetaryGroupingSeparatorSymbol, "^");
    custom.setSymbol(DecimalFormatSymbols::kMonetarySeparatorSymbol, ":");

    UnicodeString pat(" #,##0.00");
    pat.insert(0, (UChar)0x00A4);

    DecimalFormat fmt(pat, custom, status);
    CHECK(status, "DecimalFormat constructor");

    UnicodeString numstr("* 1^234:56");
    expect2(fmt, (Formattable)((double)1234.56), numstr);
}