void NumberFormatTest::TestRegCurrency(void) {
#if !UCONFIG_NO_SERVICE
    UErrorCode status = U_ZERO_ERROR;
    UChar USD[4];
    ucurr_forLocale("en_US", USD, 4, &status);
    UChar YEN[4];
    ucurr_forLocale("ja_JP", YEN, 4, &status);
    UChar TMP[4];
    static const UChar QQQ[] = {0x51, 0x51, 0x51, 0};
    if(U_FAILURE(status)) {
        errcheckln(status, "Unable to get currency for locale, error %s", u_errorName(status));
        return;
    }

    UCurrRegistryKey enkey = ucurr_register(YEN, "en_US", &status);
    UCurrRegistryKey enUSEUROkey = ucurr_register(QQQ, "en_US_EURO", &status);

    ucurr_forLocale("en_US", TMP, 4, &status);
    if (u_strcmp(YEN, TMP) != 0) {
        errln("FAIL: didn't return YEN registered for en_US");
    }

    ucurr_forLocale("en_US_EURO", TMP, 4, &status);
    if (u_strcmp(QQQ, TMP) != 0) {
        errln("FAIL: didn't return QQQ for en_US_EURO");
    }

    int32_t fallbackLen = ucurr_forLocale("en_XX_BAR", TMP, 4, &status);
    if (fallbackLen) {
        errln("FAIL: tried to fallback en_XX_BAR");
    }
    status = U_ZERO_ERROR; // reset

    if (!ucurr_unregister(enkey, &status)) {
        errln("FAIL: couldn't unregister enkey");
    }

    ucurr_forLocale("en_US", TMP, 4, &status);
    if (u_strcmp(USD, TMP) != 0) {
        errln("FAIL: didn't return USD for en_US after unregister of en_US");
    }
    status = U_ZERO_ERROR; // reset

    ucurr_forLocale("en_US_EURO", TMP, 4, &status);
    if (u_strcmp(QQQ, TMP) != 0) {
        errln("FAIL: didn't return QQQ for en_US_EURO after unregister of en_US");
    }

    ucurr_forLocale("en_US_BLAH", TMP, 4, &status);
    if (u_strcmp(USD, TMP) != 0) {
        errln("FAIL: could not find USD for en_US_BLAH after unregister of en");
    }
    status = U_ZERO_ERROR; // reset

    if (!ucurr_unregister(enUSEUROkey, &status)) {
        errln("FAIL: couldn't unregister enUSEUROkey");
    }

    ucurr_forLocale("en_US_EURO", TMP, 4, &status);
    if (u_strcmp(EUR, TMP) != 0) {
        errln("FAIL: didn't return EUR for en_US_EURO after unregister of en_US_EURO");
    }
    status = U_ZERO_ERROR; // reset
#endif
}