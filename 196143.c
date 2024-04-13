void NumberFormatTest::TestExplicitParents() {

    /* Test that number formats are properly inherited from es_419 */
    /* These could be subject to change if the CLDR data changes */
    static const char* parentLocaleTests[][2]= {
    /* locale ID */  /* expected */
    {"es_CO", "1.250,75" },
    {"es_ES", "1.250,75" },
    {"es_GQ", "1.250,75" },
    {"es_MX", "1,250.75" },
    {"es_US", "1,250.75" },
    {"es_VE", "1.250,75" },
    };

    UnicodeString s;

    for(int i=0; i < UPRV_LENGTHOF(parentLocaleTests); i++){
        UErrorCode status = U_ZERO_ERROR;
        const char *localeID = parentLocaleTests[i][0];
        UnicodeString expected(parentLocaleTests[i][1], -1, US_INV);
        expected = expected.unescape();
        char loc[256]={0};
        uloc_canonicalize(localeID, loc, 256, &status);
        NumberFormat *fmt= NumberFormat::createInstance(Locale(loc), status);
        if(U_FAILURE(status)){
            dataerrln("Could not create number formatter for locale %s - %s",localeID, u_errorName(status));
            continue;
        }
        s.remove();
        fmt->format(1250.75, s);
        if(s!=expected){
            errln(UnicodeString("FAIL: Expected: ")+expected
                    + UnicodeString(" Got: ") + s
                    + UnicodeString( " for locale: ")+ UnicodeString(localeID) );
        }
        if (U_FAILURE(status)){
            errln((UnicodeString)"FAIL: Status " + (int32_t)status);
        }
        delete fmt;
    }

}