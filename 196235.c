void NumberFormatTest::TestRoundingPattern() {
    UErrorCode status = U_ZERO_ERROR;
    struct {
        UnicodeString  pattern;
        double        testCase;
        UnicodeString expected;
    } tests[] = {
            { (UnicodeString)"##0.65", 1.234, (UnicodeString)"1.30" },
            { (UnicodeString)"#50",    1230,  (UnicodeString)"1250" }
    };
    int32_t numOfTests = UPRV_LENGTHOF(tests);
    UnicodeString result;

    DecimalFormat *df = (DecimalFormat*)NumberFormat::createCurrencyInstance(Locale::getEnglish(), status);
    if (U_FAILURE(status)) {
        dataerrln("Unable to create decimal formatter. - %s", u_errorName(status));
        return;
    }

    for (int32_t i = 0; i < numOfTests; i++) {
        result.remove();

        df->applyPattern(tests[i].pattern, status);
        if (U_FAILURE(status)) {
            errln("Unable to apply pattern to decimal formatter. - %s", u_errorName(status));
        }

        df->format(tests[i].testCase, result);

        if (result != tests[i].expected) {
            errln("String Pattern Rounding Test Failed: Pattern: \"" + tests[i].pattern + "\" Number: " + tests[i].testCase + " - Got: " + result + " Expected: " + tests[i].expected);
        }
    }

    delete df;
}