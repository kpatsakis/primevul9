NumberFormatTest::TestDecimalFormatCurrencyParse() {
    // Locale.US
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols* sym = new DecimalFormatSymbols(Locale("en_US"), status);
    if (U_FAILURE(status)) {
        delete sym;
        return;
    }
    UnicodeString pat;
    UChar currency = 0x00A4;
    // "\xA4#,##0.00;-\xA4#,##0.00"
    pat.append(currency).append(currency).append(currency).append("#,##0.00;-").append(currency).append(currency).append(currency).append("#,##0.00");
    DecimalFormat* fmt = new DecimalFormat(pat, sym, status);
    if (U_FAILURE(status)) {
        delete fmt;
        errln("failed to new DecimalFormat in TestDecimalFormatCurrencyParse");
        return;
    }
    const char* DATA[][2] = {
        // the data are:
        // string to be parsed, the parsed result (number)
        {"$1.00", "1"},
        {"USD1.00", "1"},
        {"1.00 US dollar", "1"},
        {"$1,234.56", "1234.56"},
        {"USD1,234.56", "1234.56"},
        {"1,234.56 US dollar", "1234.56"},
    };
    // NOTE: ICU 62 requires that the currency format match the pattern in strict mode.
    fmt->setLenient(TRUE);
    for (uint32_t i = 0; i < UPRV_LENGTHOF(DATA); ++i) {
        UnicodeString stringToBeParsed = ctou(DATA[i][0]);
        double parsedResult = atof(DATA[i][1]);
        UErrorCode status = U_ZERO_ERROR;
        Formattable result;
        fmt->parse(stringToBeParsed, result, status);
        logln((UnicodeString)"Input: " + stringToBeParsed + "; output: " + result.getDouble(status));
        if (U_FAILURE(status) ||
            (result.getType() == Formattable::kDouble &&
            result.getDouble() != parsedResult) ||
            (result.getType() == Formattable::kLong &&
            result.getLong() != parsedResult)) {
            errln((UnicodeString)"FAIL parse: Expected " + parsedResult);
        }
    }
    delete fmt;
}