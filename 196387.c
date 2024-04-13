void NumberFormatTest::TestFastFormatInt32() {
    IcuTestErrorCode status(*this, "TestFastFormatInt32");

    // The two simplest formatters, old API and new API.
    // Old API should use the fastpath for ints.
    LocalizedNumberFormatter lnf = NumberFormatter::withLocale("en");
    LocalPointer<NumberFormat> df(NumberFormat::createInstance("en", status), status);
    if (!assertSuccess("", status, true, __FILE__, __LINE__)) {return;}

    double nums[] = {
            0.0,
            -0.0,
            NAN,
            INFINITY,
            0.1,
            1.0,
            1.1,
            2.0,
            3.0,
            9.0,
            10.0,
            99.0,
            100.0,
            999.0,
            1000.0,
            9999.0,
            10000.0,
            99999.0,
            100000.0,
            999999.0,
            1000000.0,
            static_cast<double>(INT32_MAX) - 1,
            static_cast<double>(INT32_MAX),
            static_cast<double>(INT32_MAX) + 1,
            static_cast<double>(INT32_MIN) - 1,
            static_cast<double>(INT32_MIN),
            static_cast<double>(INT32_MIN) + 1};

    for (auto num : nums) {
        UnicodeString expected = lnf.formatDouble(num, status).toString();
        UnicodeString actual;
        df->format(num, actual);
        assertEquals(UnicodeString("d = ") + num, expected, actual);
    }
}