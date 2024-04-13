void NumberFormatTest::TestCurrencyAmount(void){
    UErrorCode ec = U_ZERO_ERROR;
    static const UChar USD[] = {85, 83, 68, 0}; /*USD*/
    CurrencyAmount ca(9, USD, ec);
    assertSuccess("CurrencyAmount", ec);

    CurrencyAmount ca2(ca);
    if (!(ca2 == ca)){
        errln("CurrencyAmount copy constructed object should be same");
    }

    ca2=ca;
    if (!(ca2 == ca)){
        errln("CurrencyAmount assigned object should be same");
    }

    CurrencyAmount *ca3 = (CurrencyAmount *)ca.clone();
    if (!(*ca3 == ca)){
        errln("CurrencyAmount cloned object should be same");
    }
    delete ca3;
}