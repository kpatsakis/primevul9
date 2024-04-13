void NumberFormatTest::TestCurrencyUsage() {
    double agent = 123.567;

    UErrorCode status;
    DecimalFormat *fmt;

    // compare the Currency and Currency Cash Digits
    // Note that as of CLDR 26:
    // * TWD and PKR switched from 0 decimals to 2; ISK still has 0, so change test to that
    // * CAD rounds to .05 in cash mode only
    // 1st time for getter/setter, 2nd time for factory method
    Locale enUS_ISK("en_US@currency=ISK");

    for(int i=0; i<2; i++){
        status = U_ZERO_ERROR;
        if(i == 0){
            fmt = (DecimalFormat *) NumberFormat::createInstance(enUS_ISK, UNUM_CURRENCY, status);
            if (assertSuccess("en_US@currency=ISK/CURRENCY", status, TRUE) == FALSE) {
                continue;
            }

            UnicodeString original;
            fmt->format(agent,original);
            assertEquals("Test Currency Usage 1", u"ISK\u00A0124", original);

            // test the getter here
            UCurrencyUsage curUsage = fmt->getCurrencyUsage();
            assertEquals("Test usage getter - standard", (int32_t)curUsage, (int32_t)UCURR_USAGE_STANDARD);

            fmt->setCurrencyUsage(UCURR_USAGE_CASH, &status);
        }else{
            fmt = (DecimalFormat *) NumberFormat::createInstance(enUS_ISK, UNUM_CASH_CURRENCY, status);
            if (assertSuccess("en_US@currency=ISK/CASH", status, TRUE) == FALSE) {
                continue;
            }
        }

        // must be usage = cash
        UCurrencyUsage curUsage = fmt->getCurrencyUsage();
        assertEquals("Test usage getter - cash", (int32_t)curUsage, (int32_t)UCURR_USAGE_CASH);

        UnicodeString cash_currency;
        fmt->format(agent,cash_currency);
        assertEquals("Test Currency Usage 2", u"ISK\u00A0124", cash_currency);
        delete fmt;
    }

    // compare the Currency and Currency Cash Rounding
    // 1st time for getter/setter, 2nd time for factory method
    Locale enUS_CAD("en_US@currency=CAD");
    for(int i=0; i<2; i++){
        status = U_ZERO_ERROR;
        if(i == 0){
            fmt = (DecimalFormat *) NumberFormat::createInstance(enUS_CAD, UNUM_CURRENCY, status);
            if (assertSuccess("en_US@currency=CAD/CURRENCY", status, TRUE) == FALSE) {
                continue;
            }

            UnicodeString original_rounding;
            fmt->format(agent, original_rounding);
            assertEquals("Test Currency Usage 3", u"CA$123.57", original_rounding);
            fmt->setCurrencyUsage(UCURR_USAGE_CASH, &status);
        }else{
            fmt = (DecimalFormat *) NumberFormat::createInstance(enUS_CAD, UNUM_CASH_CURRENCY, status);
            if (assertSuccess("en_US@currency=CAD/CASH", status, TRUE) == FALSE) {
                continue;
            }
        }

        UnicodeString cash_rounding_currency;
        fmt->format(agent, cash_rounding_currency);
        assertEquals("Test Currency Usage 4", u"CA$123.55", cash_rounding_currency);
        delete fmt;
    }

    // Test the currency change
    // 1st time for getter/setter, 2nd time for factory method
    const UChar CUR_PKR[] = {0x50, 0x4B, 0x52, 0};
    for(int i=0; i<2; i++){
        status = U_ZERO_ERROR;
        if(i == 0){
            fmt = (DecimalFormat *) NumberFormat::createInstance(enUS_CAD, UNUM_CURRENCY, status);
            if (assertSuccess("en_US@currency=CAD/CURRENCY", status, TRUE) == FALSE) {
                continue;
            }
            fmt->setCurrencyUsage(UCURR_USAGE_CASH, &status);
        }else{
            fmt = (DecimalFormat *) NumberFormat::createInstance(enUS_CAD, UNUM_CASH_CURRENCY, status);
            if (assertSuccess("en_US@currency=CAD/CASH", status, TRUE) == FALSE) {
                continue;
            }
        }

        UnicodeString cur_original;
        fmt->setCurrencyUsage(UCURR_USAGE_STANDARD, &status);
        fmt->format(agent, cur_original);
        assertEquals("Test Currency Usage 5", u"CA$123.57", cur_original);

        fmt->setCurrency(CUR_PKR, status);
        assertSuccess("Set currency to PKR", status);

        UnicodeString PKR_changed;
        fmt->format(agent, PKR_changed);
        assertEquals("Test Currency Usage 6", u"PKR\u00A0123.57", PKR_changed);
        delete fmt;
    }
}