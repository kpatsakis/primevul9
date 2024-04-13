void NumberFormatTest::TestCurrencyFormat()
{
    // This test is here to increase code coverage.
    UErrorCode status = U_ZERO_ERROR;
    MeasureFormat *cloneObj;
    UnicodeString str;
    Formattable toFormat, result;
    static const UChar ISO_CODE[4] = {0x0047, 0x0042, 0x0050, 0};

    Locale  saveDefaultLocale = Locale::getDefault();
    Locale::setDefault( Locale::getUK(), status );
    if (U_FAILURE(status)) {
        errln("couldn't set default Locale!");
        return;
    }

    MeasureFormat *measureObj = MeasureFormat::createCurrencyFormat(status);
    Locale::setDefault( saveDefaultLocale, status );
    if (U_FAILURE(status)){
        dataerrln("FAIL: Status %s", u_errorName(status));
        return;
    }
    cloneObj = (MeasureFormat *)measureObj->clone();
    if (cloneObj == NULL) {
        errln("Clone doesn't work");
        return;
    }
    toFormat.adoptObject(new CurrencyAmount(1234.56, ISO_CODE, status));
    measureObj->format(toFormat, str, status);
    measureObj->parseObject(str, result, status);
    if (U_FAILURE(status)){
        errln("FAIL: Status %s", u_errorName(status));
    }
    if (result != toFormat) {
        errln("measureObj does not round trip. Formatted string was \"" + str + "\" Got: " + toString(result) + " Expected: " + toString(toFormat));
    }
    status = U_ZERO_ERROR;
    str.truncate(0);
    cloneObj->format(toFormat, str, status);
    cloneObj->parseObject(str, result, status);
    if (U_FAILURE(status)){
        errln("FAIL: Status %s", u_errorName(status));
    }
    if (result != toFormat) {
        errln("Clone does not round trip. Formatted string was \"" + str + "\" Got: " + toString(result) + " Expected: " + toString(toFormat));
    }
    if (*measureObj != *cloneObj) {
        errln("Cloned object is not equal to the original object");
    }
    delete measureObj;
    delete cloneObj;

    status = U_USELESS_COLLATOR_ERROR;
    if (MeasureFormat::createCurrencyFormat(status) != NULL) {
        errln("createCurrencyFormat should have returned NULL.");
    }
}