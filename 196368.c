void NumberFormatTest::TestJB3832(){
    const char* localeID = "pt_PT@currency=PTE";
    Locale loc(localeID);
    UErrorCode status = U_ZERO_ERROR;
    UnicodeString expected(CharsToUnicodeString("1,150$50\\u00A0\\u200B")); // per cldrbug 7670
    UnicodeString s;
    NumberFormat* currencyFmt = NumberFormat::createCurrencyInstance(loc, status);
    if(U_FAILURE(status)){
        dataerrln("Could not create currency formatter for locale %s - %s", localeID, u_errorName(status));
        return;
    }
    currencyFmt->format(1150.50, s);
    if(s!=expected){
        errln(UnicodeString("FAIL: Expected: ")+expected
                + UnicodeString(" Got: ") + s
                + UnicodeString( " for locale: ")+ UnicodeString(localeID) );
    }
    if (U_FAILURE(status)){
        errln("FAIL: Status %s", u_errorName(status));
    }
    delete currencyFmt;
}