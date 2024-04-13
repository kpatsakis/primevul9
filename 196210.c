void NumberFormatTest::Test11648_ExpDecFormatMalPattern() {
    UErrorCode status = U_ZERO_ERROR;

    DecimalFormat fmt("0.00", {"en", status}, status);
    if (!assertSuccess("", status, true, __FILE__, __LINE__)) { return; }
    fmt.setScientificNotation(TRUE);
    UnicodeString pattern;

    assertEquals("A valid scientific notation pattern should be produced",
            "0.00E0",
            fmt.toPattern(pattern));

    DecimalFormat fmt2(pattern, status);
    assertSuccess("", status);
}