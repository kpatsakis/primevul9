void NumberFormatTest::TestRoundingScientific10542() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormat format("0.00E0", status);
    if (U_FAILURE(status)) {
        errcheckln(status, "DecimalFormat constructor failed - %s", u_errorName(status));
        return;
    }

    DecimalFormat::ERoundingMode roundingModes[] = {
            DecimalFormat::kRoundCeiling,
            DecimalFormat::kRoundDown,
            DecimalFormat::kRoundFloor,
            DecimalFormat::kRoundHalfDown,
            DecimalFormat::kRoundHalfEven,
            DecimalFormat::kRoundHalfUp,
            DecimalFormat::kRoundUp};
    const char *descriptions[] = {
            "Round Ceiling",
            "Round Down",
            "Round Floor",
            "Round half down",
            "Round half even",
            "Round half up",
            "Round up"};

    {
        double values[] = {-0.003006, -0.003005, -0.003004, 0.003014, 0.003015, 0.003016};
        // The order of these expected values correspond to the order of roundingModes and the order of values.
        const char *expected[] = {
                "-3.00E-3", "-3.00E-3", "-3.00E-3", "3.02E-3", "3.02E-3", "3.02E-3",
                "-3.00E-3", "-3.00E-3", "-3.00E-3", "3.01E-3", "3.01E-3", "3.01E-3",
                "-3.01E-3", "-3.01E-3", "-3.01E-3", "3.01E-3", "3.01E-3", "3.01E-3",
                "-3.01E-3", "-3.00E-3", "-3.00E-3", "3.01E-3", "3.01E-3", "3.02E-3",
                "-3.01E-3", "-3.00E-3", "-3.00E-3", "3.01E-3", "3.02E-3", "3.02E-3",
                "-3.01E-3", "-3.01E-3", "-3.00E-3", "3.01E-3", "3.02E-3", "3.02E-3",
                "-3.01E-3", "-3.01E-3", "-3.01E-3", "3.02E-3", "3.02E-3", "3.02E-3"};
        verifyRounding(
                format,
                values,
                expected,
                roundingModes,
                descriptions,
                UPRV_LENGTHOF(values),
                UPRV_LENGTHOF(roundingModes));
    }
    {
        double values[] = {-3006.0, -3005, -3004, 3014, 3015, 3016};
        // The order of these expected values correspond to the order of roundingModes and the order of values.
        const char *expected[] = {
                "-3.00E3", "-3.00E3", "-3.00E3", "3.02E3", "3.02E3", "3.02E3",
                "-3.00E3", "-3.00E3", "-3.00E3", "3.01E3", "3.01E3", "3.01E3",
                "-3.01E3", "-3.01E3", "-3.01E3", "3.01E3", "3.01E3", "3.01E3",
                "-3.01E3", "-3.00E3", "-3.00E3", "3.01E3", "3.01E3", "3.02E3",
                "-3.01E3", "-3.00E3", "-3.00E3", "3.01E3", "3.02E3", "3.02E3",
                "-3.01E3", "-3.01E3", "-3.00E3", "3.01E3", "3.02E3", "3.02E3",
                "-3.01E3", "-3.01E3", "-3.01E3", "3.02E3", "3.02E3", "3.02E3"};
        verifyRounding(
                format,
                values,
                expected,
                roundingModes,
                descriptions,
                UPRV_LENGTHOF(values),
                UPRV_LENGTHOF(roundingModes));
    }
/* Commented out for now until we decide how rounding to zero should work, +0 vs. -0
    {
        double values[] = {0.0, -0.0};
        // The order of these expected values correspond to the order of roundingModes and the order of values.
        const char *expected[] = {
                "0.00E0", "-0.00E0",
                "0.00E0", "-0.00E0",
                "0.00E0", "-0.00E0",
                "0.00E0", "-0.00E0",
                "0.00E0", "-0.00E0",
                "0.00E0", "-0.00E0",
                "0.00E0", "-0.00E0"};
        verifyRounding(
                format,
                values,
                expected,
                roundingModes,
                descriptions,
                UPRV_LENGTHOF(values),
                UPRV_LENGTHOF(roundingModes));
    }
*/
    {

        double values[] = {1e25, 1e25 + 1e15, 1e25 - 1e15};
        // The order of these expected values correspond to the order of roundingModes and the order of values.
        const char *expected[] = {
                "1.00E25", "1.01E25", "1.00E25",
                "1.00E25", "1.00E25", "9.99E24",
                "1.00E25", "1.00E25", "9.99E24",
                "1.00E25", "1.00E25", "1.00E25",
                "1.00E25", "1.00E25", "1.00E25",
                "1.00E25", "1.00E25", "1.00E25",
                "1.00E25", "1.01E25", "1.00E25"};
        verifyRounding(
                format,
                values,
                expected,
                roundingModes,
                descriptions,
                UPRV_LENGTHOF(values),
                UPRV_LENGTHOF(roundingModes));
        }
    {
        double values[] = {-1e25, -1e25 + 1e15, -1e25 - 1e15};
        // The order of these expected values correspond to the order of roundingModes and the order of values.
        const char *expected[] = {
                "-1.00E25", "-9.99E24", "-1.00E25",
                "-1.00E25", "-9.99E24", "-1.00E25",
                "-1.00E25", "-1.00E25", "-1.01E25",
                "-1.00E25", "-1.00E25", "-1.00E25",
                "-1.00E25", "-1.00E25", "-1.00E25",
                "-1.00E25", "-1.00E25", "-1.00E25",
                "-1.00E25", "-1.00E25", "-1.01E25"};
        verifyRounding(
                format,
                values,
                expected,
                roundingModes,
                descriptions,
                UPRV_LENGTHOF(values),
                UPRV_LENGTHOF(roundingModes));
        }
    {
        double values[] = {1e-25, 1e-25 + 1e-35, 1e-25 - 1e-35};
        // The order of these expected values correspond to the order of roundingModes and the order of values.
        const char *expected[] = {
                "1.00E-25", "1.01E-25", "1.00E-25",
                "1.00E-25", "1.00E-25", "9.99E-26",
                "1.00E-25", "1.00E-25", "9.99E-26",
                "1.00E-25", "1.00E-25", "1.00E-25",
                "1.00E-25", "1.00E-25", "1.00E-25",
                "1.00E-25", "1.00E-25", "1.00E-25",
                "1.00E-25", "1.01E-25", "1.00E-25"};
        verifyRounding(
                format,
                values,
                expected,
                roundingModes,
                descriptions,
                UPRV_LENGTHOF(values),
                UPRV_LENGTHOF(roundingModes));
        }
    {
        double values[] = {-1e-25, -1e-25 + 1e-35, -1e-25 - 1e-35};
        // The order of these expected values correspond to the order of roundingModes and the order of values.
        const char *expected[] = {
                "-1.00E-25", "-9.99E-26", "-1.00E-25",
                "-1.00E-25", "-9.99E-26", "-1.00E-25",
                "-1.00E-25", "-1.00E-25", "-1.01E-25",
                "-1.00E-25", "-1.00E-25", "-1.00E-25",
                "-1.00E-25", "-1.00E-25", "-1.00E-25",
                "-1.00E-25", "-1.00E-25", "-1.00E-25",
                "-1.00E-25", "-1.00E-25", "-1.01E-25"};
        verifyRounding(
                format,
                values,
                expected,
                roundingModes,
                descriptions,
                UPRV_LENGTHOF(values),
                UPRV_LENGTHOF(roundingModes));
    }
}