void NumberFormatTest::Test13850_EmptyStringCurrency() {
    IcuTestErrorCode status(*this, "Test13840_EmptyStringCurrency");

    struct TestCase {
        const char16_t* currencyArg;
        UErrorCode expectedError;
    } cases[] = {
        {u"", U_ZERO_ERROR},
        {u"U", U_ILLEGAL_ARGUMENT_ERROR},
        {u"Us", U_ILLEGAL_ARGUMENT_ERROR},
        {nullptr, U_ZERO_ERROR},
        {u"U$D", U_INVARIANT_CONVERSION_ERROR},
        {u"Xxx", U_ZERO_ERROR}
    };
    for (const auto& cas : cases) {
        UnicodeString message(u"with currency arg: ");
        if (cas.currencyArg == nullptr) {
            message += u"nullptr";
        } else {
            message += UnicodeString(cas.currencyArg);
        }
        status.setScope(message);
        LocalPointer<NumberFormat> nf(NumberFormat::createCurrencyInstance("en-US", status), status);
        if (status.errIfFailureAndReset()) { return; }
        UnicodeString actual;
        nf->format(1, actual, status);
        status.errIfFailureAndReset();
        assertEquals(u"Should format with US currency " + message, u"$1.00", actual);
        nf->setCurrency(cas.currencyArg, status);
        if (status.expectErrorAndReset(cas.expectedError)) {
            // If an error occurred, do not check formatting.
            continue;
        }
        nf->format(1, actual.remove(), status);
        assertEquals(u"Should unset the currency " + message, u"\u00A41.00", actual);
        status.errIfFailureAndReset();
    }
}