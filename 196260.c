static void parseCurrencyAmount(const UnicodeString& str,
                                const NumberFormat& fmt,
                                UChar delim,
                                Formattable& result,
                                UErrorCode& ec) {
    UnicodeString num, cur;
    int32_t i = str.indexOf(delim);
    str.extractBetween(0, i, num);
    str.extractBetween(i+1, INT32_MAX, cur);
    Formattable n;
    fmt.parse(num, n, ec);
    result.adoptObject(new CurrencyAmount(n, cur.getTerminatedBuffer(), ec));
}