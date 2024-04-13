void NumberFormatTest::Test13327_numberingSystemBufferOverflow() {
    UErrorCode status = U_ZERO_ERROR;
    for (int runId = 0; runId < 2; runId++) {
        // Construct a locale string with a very long "numbers" value.
        // The first time, make the value length exactly equal to ULOC_KEYWORDS_CAPACITY.
        // The second time, make it exceed ULOC_KEYWORDS_CAPACITY.
        int extraLength = (runId == 0) ? 0 : 5;

        CharString localeId("en@numbers=", status);
        for (int i = 0; i < ULOC_KEYWORDS_CAPACITY + extraLength; i++) {
            localeId.append('x', status);
        }
        assertSuccess("Constructing locale string", status);
        Locale locale(localeId.data());

        LocalPointer<NumberingSystem> ns(NumberingSystem::createInstance(locale, status));
        assertFalse("Should not be null", ns.getAlias() == nullptr);
        assertSuccess("Should create with no error", status);
    }
}