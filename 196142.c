void NumberFormatTest::TestIllegalPatterns() {
    // Test cases:
    // Prefix with "-:" for illegal patterns
    // Prefix with "+:" for legal patterns
    const char* DATA[] = {
        // Unquoted special characters in the suffix are illegal
        "-:000.000|###",
        "+:000.000'|###'",
        0
    };
    for (int32_t i=0; DATA[i]; ++i) {
        const char* pat=DATA[i];
        UBool valid = (*pat) == '+';
        pat += 2;
        UErrorCode ec = U_ZERO_ERROR;
        DecimalFormat fmt(pat, ec); // locale doesn't matter here
        if (U_SUCCESS(ec) == valid) {
            logln("Ok: pattern \"%s\": %s",
                  pat, u_errorName(ec));
        } else {
            errcheckln(ec, "FAIL: pattern \"%s\" should have %s; got %s",
                  pat, (valid?"succeeded":"failed"),
                  u_errorName(ec));
        }
    }
}