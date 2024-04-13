void NumberFormatTest::TestEquality() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols symbols(Locale("root"), status);
    if (U_FAILURE(status)) {
    	dataerrln("Fail: can't create DecimalFormatSymbols for root");
    	return;
    }
    UnicodeString pattern("#,##0.###");
    DecimalFormat fmtBase(pattern, symbols, status);
    if (U_FAILURE(status)) {
    	dataerrln("Fail: can't create DecimalFormat using root symbols");
    	return;
    }

    DecimalFormat* fmtClone = (DecimalFormat*)fmtBase.clone();
    fmtClone->setFormatWidth(fmtBase.getFormatWidth() + 32);
    if (*fmtClone == fmtBase) {
        errln("Error: DecimalFormat == does not distinguish objects that differ only in FormatWidth");
    }
    delete fmtClone;
}