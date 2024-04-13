NumberFormatTest::TestCurrency(void)
{
    UErrorCode status = U_ZERO_ERROR;
    NumberFormat* currencyFmt = NumberFormat::createCurrencyInstance(Locale::getCanadaFrench(), status);
    if (U_FAILURE(status)) {
        dataerrln("Error calling NumberFormat::createCurrencyInstance()");
        return;
    }

    UnicodeString s; currencyFmt->format(1.50, s);
    logln((UnicodeString)"Un pauvre ici a..........." + s);
    if (!(s==CharsToUnicodeString("1,50\\u00A0$")))
        errln((UnicodeString)"FAIL: Expected 1,50<nbsp>$ but got " + s);
    delete currencyFmt;
    s.truncate(0);
    char loc[256]={0};
    int len = uloc_canonicalize("de_DE_PREEURO", loc, 256, &status);
    (void)len;  // Suppress unused variable warning.
    currencyFmt = NumberFormat::createCurrencyInstance(Locale(loc),status);
    currencyFmt->format(1.50, s);
    logln((UnicodeString)"Un pauvre en Allemagne a.." + s);
    if (!(s==CharsToUnicodeString("1,50\\u00A0DM")))
        errln((UnicodeString)"FAIL: Expected 1,50<nbsp>DM but got " + s);
    delete currencyFmt;
    s.truncate(0);
    len = uloc_canonicalize("fr_FR_PREEURO", loc, 256, &status);
    currencyFmt = NumberFormat::createCurrencyInstance(Locale(loc), status);
    currencyFmt->format(1.50, s);
    logln((UnicodeString)"Un pauvre en France a....." + s);
    if (!(s==CharsToUnicodeString("1,50\\u00A0F")))
        errln((UnicodeString)"FAIL: Expected 1,50<nbsp>F");
    delete currencyFmt;
    if (U_FAILURE(status))
        errln((UnicodeString)"FAIL: Status " + (int32_t)status);

    for(int i=0; i < UPRV_LENGTHOF(testCases); i++){
        status = U_ZERO_ERROR;
        const char *localeID = testCases[i][0];
        UnicodeString expected(testCases[i][1], -1, US_INV);
        expected = expected.unescape();
        s.truncate(0);
        char loc[256]={0};
        uloc_canonicalize(localeID, loc, 256, &status);
        currencyFmt = NumberFormat::createCurrencyInstance(Locale(loc), status);
        if(U_FAILURE(status)){
            errln("Could not create currency formatter for locale %s",localeID);
            continue;
        }
        currencyFmt->format(1150.50, s);
        if(s!=expected){
            errln(UnicodeString("FAIL: Expected: ")+expected
                    + UnicodeString(" Got: ") + s
                    + UnicodeString( " for locale: ")+ UnicodeString(localeID) );
        }
        if (U_FAILURE(status)){
            errln((UnicodeString)"FAIL: Status " + (int32_t)status);
        }
        delete currencyFmt;
    }
}