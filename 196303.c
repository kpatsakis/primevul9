NumberFormatTest::Test9087(void)
{
    U_STRING_DECL(pattern,"#",1);
    U_STRING_INIT(pattern,"#",1);

    U_STRING_DECL(infstr,"INF",3);
    U_STRING_INIT(infstr,"INF",3);

    U_STRING_DECL(nanstr,"NAN",3);
    U_STRING_INIT(nanstr,"NAN",3);

    UChar outputbuf[50] = {0};
    UErrorCode status = U_ZERO_ERROR;
    UNumberFormat* fmt = unum_open(UNUM_PATTERN_DECIMAL,pattern,1,NULL,NULL,&status);
    if ( U_FAILURE(status) ) {
        dataerrln("FAIL: error in unum_open() - %s", u_errorName(status));
        return;
    }

    unum_setSymbol(fmt,UNUM_INFINITY_SYMBOL,infstr,3,&status);
    unum_setSymbol(fmt,UNUM_NAN_SYMBOL,nanstr,3,&status);
    if ( U_FAILURE(status) ) {
        errln("FAIL: error setting symbols");
    }

    double inf = uprv_getInfinity();

    unum_setAttribute(fmt,UNUM_ROUNDING_MODE,UNUM_ROUND_HALFEVEN);
    unum_setDoubleAttribute(fmt,UNUM_ROUNDING_INCREMENT,0);

    UFieldPosition position = { 0, 0, 0};
    unum_formatDouble(fmt,inf,outputbuf,50,&position,&status);

    if ( u_strcmp(infstr, outputbuf)) {
        errln((UnicodeString)"FAIL: unexpected result for infinity - expected " + infstr + " got " + outputbuf);
    }

    unum_close(fmt);
}