void NumberFormatTest::TestExponentParse() {

    UErrorCode status = U_ZERO_ERROR;
    Formattable result;
    ParsePosition parsePos(0);

    // set the exponent symbol
    status = U_ZERO_ERROR;
    DecimalFormatSymbols symbols(Locale::getDefault(), status);
    if(U_FAILURE(status)) {
        dataerrln((UnicodeString)"ERROR: Could not create DecimalFormatSymbols (Default)");
        return;
    }

    // create format instance
    status = U_ZERO_ERROR;
    DecimalFormat fmt(u"#####", symbols, status);
    if(U_FAILURE(status)) {
        errln((UnicodeString)"ERROR: Could not create DecimalFormat (pattern, symbols*)");
    }

    // parse the text
    fmt.parse("5.06e-27", result, parsePos);
    if(result.getType() != Formattable::kDouble &&
       result.getDouble() != 5.06E-27 &&
       parsePos.getIndex() != 8
       )
    {
        errln("ERROR: parse failed - expected 5.06E-27, 8  - returned %d, %i",
              result.getDouble(), parsePos.getIndex());
    }
}