void NumberFormatTest::TestHost()
{
#if U_PLATFORM_USES_ONLY_WIN32_API
    Win32NumberTest::testLocales(this);
#endif
    Locale loc("en_US@compat=host");
    for (UNumberFormatStyle k = UNUM_DECIMAL;
         k < UNUM_FORMAT_STYLE_COUNT; k = (UNumberFormatStyle)(k+1)) {
        UErrorCode status = U_ZERO_ERROR;
        LocalPointer<NumberFormat> full(NumberFormat::createInstance(loc, k, status));
        if (!NumberFormat::isStyleSupported(k)) {
            if (status != U_UNSUPPORTED_ERROR) {
                errln("FAIL: expected style %d to be unsupported - %s",
                      k, u_errorName(status));
            }
            continue;
        }
        if (full.isNull() || U_FAILURE(status)) {
            dataerrln("FAIL: Can't create number instance of style %d for host - %s",
                      k, u_errorName(status));
            return;
        }
        UnicodeString result1;
        Formattable number(10.00);
        full->format(number, result1, status);
        if (U_FAILURE(status)) {
            errln("FAIL: Can't format for host");
            return;
        }
        Formattable formattable;
        full->parse(result1, formattable, status);
        if (U_FAILURE(status)) {
            errln("FAIL: Can't parse for host");
            return;
        }
    }
}