void NumberFormatTest::TestNonpositiveMultiplier() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols US(Locale::getUS(), status);
    CHECK(status, "DecimalFormatSymbols constructor");
    DecimalFormat df(UnicodeString("0"), US, status);
    CHECK(status, "DecimalFormat(0)");

    // test zero multiplier

    int32_t mult = df.getMultiplier();
    df.setMultiplier(0);
    if (df.getMultiplier() != mult) {
        errln("DecimalFormat.setMultiplier(0) did not ignore its zero input");
    }

    // test negative multiplier

    df.setMultiplier(-1);
    if (df.getMultiplier() != -1) {
        errln("DecimalFormat.setMultiplier(-1) ignored its negative input");
        return;
    }

    expect(df, "1122.123", -1122.123);
    expect(df, "-1122.123", 1122.123);
    expect(df, "1.2", -1.2);
    expect(df, "-1.2", 1.2);

    // Note:  the tests with the final parameter of FALSE will not round trip.
    //        The initial numeric value will format correctly, after the multiplier.
    //        Parsing the formatted text will be out-of-range for an int64, however.
    //        The expect() function could be modified to detect this and fall back
    //        to looking at the decimal parsed value, but it doesn't.
    expect(df, U_INT64_MIN,    "9223372036854775808", FALSE);
    expect(df, U_INT64_MIN+1,  "9223372036854775807");
    expect(df, (int64_t)-123,                  "123");
    expect(df, (int64_t)123,                  "-123");
    expect(df, U_INT64_MAX-1, "-9223372036854775806");
    expect(df, U_INT64_MAX,   "-9223372036854775807");

    df.setMultiplier(-2);
    expect(df, -(U_INT64_MIN/2)-1, "-9223372036854775806");
    expect(df, -(U_INT64_MIN/2),   "-9223372036854775808");
    expect(df, -(U_INT64_MIN/2)+1, "-9223372036854775810", FALSE);

    df.setMultiplier(-7);
    expect(df, -(U_INT64_MAX/7)-1, "9223372036854775814", FALSE);
    expect(df, -(U_INT64_MAX/7),   "9223372036854775807");
    expect(df, -(U_INT64_MAX/7)+1, "9223372036854775800");

    // TODO: uncomment (and fix up) all the following int64_t tests once BigInteger is ported
    // (right now the big numbers get turned into doubles and lose tons of accuracy)
    //expect2(df, U_INT64_MAX, Int64ToUnicodeString(-U_INT64_MAX));
    //expect2(df, U_INT64_MIN, UnicodeString(Int64ToUnicodeString(U_INT64_MIN), 1));
    //expect2(df, U_INT64_MAX / 2, Int64ToUnicodeString(-(U_INT64_MAX / 2)));
    //expect2(df, U_INT64_MIN / 2, Int64ToUnicodeString(-(U_INT64_MIN / 2)));

    // TODO: uncomment (and fix up) once BigDecimal is ported and DecimalFormat can handle it
    //expect2(df, BigDecimal.valueOf(Long.MAX_VALUE), BigDecimal.valueOf(Long.MAX_VALUE).negate().toString());
    //expect2(df, BigDecimal.valueOf(Long.MIN_VALUE), BigDecimal.valueOf(Long.MIN_VALUE).negate().toString());
    //expect2(df, java.math.BigDecimal.valueOf(Long.MAX_VALUE), java.math.BigDecimal.valueOf(Long.MAX_VALUE).negate().toString());
    //expect2(df, java.math.BigDecimal.valueOf(Long.MIN_VALUE), java.math.BigDecimal.valueOf(Long.MIN_VALUE).negate().toString());
}