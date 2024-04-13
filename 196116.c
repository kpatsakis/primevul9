void NumberFormatTest::TestCurrencyPatterns(void) {
    int32_t i, locCount;
    const Locale* locs = NumberFormat::getAvailableLocales(locCount);
    for (i=0; i<locCount; ++i) {
        UErrorCode ec = U_ZERO_ERROR;
        NumberFormat* nf = NumberFormat::createCurrencyInstance(locs[i], ec);
        if (U_FAILURE(ec)) {
            errln("FAIL: Can't create NumberFormat(%s) - %s", locs[i].getName(), u_errorName(ec));
        } else {
            // Make sure currency formats do not have a variable number
            // of fraction digits
            int32_t min = nf->getMinimumFractionDigits();
            int32_t max = nf->getMaximumFractionDigits();
            if (min != max) {
                UnicodeString a, b;
                nf->format(1.0, a);
                nf->format(1.125, b);
                errln((UnicodeString)"FAIL: " + locs[i].getName() +
                      " min fraction digits != max fraction digits; "
                      "x 1.0 => " + escape(a) +
                      "; x 1.125 => " + escape(b));
            }

            // Make sure EURO currency formats have exactly 2 fraction digits
            DecimalFormat* df = dynamic_cast<DecimalFormat*>(nf);
            if (df != NULL) {
                if (u_strcmp(EUR, df->getCurrency()) == 0) {
                    if (min != 2 || max != 2) {
                        UnicodeString a;
                        nf->format(1.0, a);
                        errln((UnicodeString)"FAIL: " + locs[i].getName() +
                              " is a EURO format but it does not have 2 fraction digits; "
                              "x 1.0 => " +
                              escape(a));
                    }
                }
            }
        }
        delete nf;
    }
}