void NumberFormatTest::TestSymbolsWithBadLocale(void) {
    Locale locDefault;
    static const char *badLocales[] = {
        // length < ULOC_FULLNAME_CAPACITY
        "x-crazy_ZZ_MY_SPECIAL_ADMINISTRATION_REGION_NEEDS_A_SPECIAL_VARIANT_WITH_A_REALLY_REALLY_REALLY_REALLY_REALLY_REALLY_REALLY_LONG_NAME",

        // length > ULOC_FULLNAME_CAPACITY
        "x-crazy_ZZ_MY_SPECIAL_ADMINISTRATION_REGION_NEEDS_A_SPECIAL_VARIANT_WITH_A_REALLY_REALLY_REALLY_REALLY_REALLY_REALLY_REALLY_REALLY_REALLY_REALLY_REALLY_LONG_NAME"
    }; // expect U_USING_DEFAULT_WARNING for both

    unsigned int i;
    for (i = 0; i < UPRV_LENGTHOF(badLocales); i++) {
        const char *localeName = badLocales[i];
        Locale locBad(localeName);
        TEST_ASSERT_TRUE(!locBad.isBogus());
        UErrorCode status = U_ZERO_ERROR;
        UnicodeString intlCurrencySymbol((UChar)0xa4);

        intlCurrencySymbol.append((UChar)0xa4);

        logln("Current locale is %s", Locale::getDefault().getName());
        Locale::setDefault(locBad, status);
        logln("Current locale is %s", Locale::getDefault().getName());
        DecimalFormatSymbols mySymbols(status);
        if (status != U_USING_DEFAULT_WARNING) {
            errln("DecimalFormatSymbols should return U_USING_DEFAULT_WARNING.");
        }
        if (strcmp(mySymbols.getLocale().getName(), locBad.getName()) != 0) {
            errln("DecimalFormatSymbols does not have the right locale.", locBad.getName());
        }
        int symbolEnum = (int)DecimalFormatSymbols::kDecimalSeparatorSymbol;
        for (; symbolEnum < (int)DecimalFormatSymbols::kFormatSymbolCount; symbolEnum++) {
            UnicodeString symbolString = mySymbols.getSymbol((DecimalFormatSymbols::ENumberFormatSymbol)symbolEnum);
            logln(UnicodeString("DecimalFormatSymbols[") + symbolEnum + UnicodeString("] = ") + prettify(symbolString));
            if (symbolString.length() == 0
                && symbolEnum != (int)DecimalFormatSymbols::kGroupingSeparatorSymbol
                && symbolEnum != (int)DecimalFormatSymbols::kMonetaryGroupingSeparatorSymbol)
            {
                errln("DecimalFormatSymbols has an empty string at index %d.", symbolEnum);
            }
        }

        status = U_ZERO_ERROR;
        Locale::setDefault(locDefault, status);
        logln("Current locale is %s", Locale::getDefault().getName());
    }
}