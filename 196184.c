double NumberFormatTest::checkRound(DecimalFormat* df, double iValue, double lastParsed) {
    UErrorCode status=U_ZERO_ERROR;
    UnicodeString formattedDecimal;
    double parsed;
    Formattable result;
    df->format(iValue, formattedDecimal, status);

    if (U_FAILURE(status)) {
        errln("Error formatting number.");
    }

    df->parse(formattedDecimal, result, status);

    if (U_FAILURE(status)) {
        errln("Error parsing number.");
    }

    parsed=result.getDouble();

    if (lastParsed>parsed) {
        errln("Rounding wrong direction! %d > %d", lastParsed, parsed);
    }

    return lastParsed;
}