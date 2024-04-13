void NumberFormatTest::Test10354() {
    IcuTestErrorCode errorCode(*this, "Test10354");
    // Ticket #10354: invalid FieldPositionIterator when formatting with empty NaN
    DecimalFormatSymbols dfs(errorCode);
    UnicodeString empty;
    dfs.setSymbol(DecimalFormatSymbols::kNaNSymbol, empty);
    DecimalFormat df(errorCode);
    df.setDecimalFormatSymbols(dfs);
    UnicodeString result;
    FieldPositionIterator positions;
    df.format(NAN, result, &positions, errorCode);
    errorCode.errIfFailureAndReset("DecimalFormat.format(NAN, FieldPositionIterator) failed");
    FieldPosition fp;
    while (positions.next(fp)) {
        // Should not loop forever
    }
}