void NumberFormatTest::TestBenchmark() {
/*
    UErrorCode status = U_ZERO_ERROR;
    Locale en("en");
    DecimalFormatSymbols sym(en, status);
    DecimalFormat fmt("0.0000000", new DecimalFormatSymbols(sym), status);
//    DecimalFormat fmt("0.00000E0", new DecimalFormatSymbols(sym), status);
//    DecimalFormat fmt("0", new DecimalFormatSymbols(sym), status);
    FieldPosition fpos(FieldPosition::DONT_CARE);
    clock_t start = clock();
    for (int32_t i = 0; i < 1000000; ++i) {
        UnicodeString append;
        fmt.format(3.0, append, fpos, status);
//        fmt.format(4.6692016, append, fpos, status);
//        fmt.format(1234567.8901, append, fpos, status);
//        fmt.format(2.99792458E8, append, fpos, status);
//        fmt.format(31, append);
    }
    errln("Took %f", (double) (clock() - start) / CLOCKS_PER_SEC);
    assertSuccess("", status);

    UErrorCode status = U_ZERO_ERROR;
    MessageFormat fmt("{0, plural, one {I have # friend.} other {I have # friends.}}", status);
    FieldPosition fpos(FieldPosition::DONT_CARE);
    Formattable one(1.0);
    Formattable three(3.0);
    clock_t start = clock();
    for (int32_t i = 0; i < 500000; ++i) {
        UnicodeString append;
        fmt.format(&one, 1, append, fpos, status);
        UnicodeString append2;
        fmt.format(&three, 1, append2, fpos, status);
    }
    errln("Took %f", (double) (clock() - start) / CLOCKS_PER_SEC);
    assertSuccess("", status);

    UErrorCode status = U_ZERO_ERROR;
    Locale en("en");
    Measure measureC(23, MeasureUnit::createCelsius(status), status);
    MeasureFormat fmt(en, UMEASFMT_WIDTH_WIDE, status);
    FieldPosition fpos(FieldPosition::DONT_CARE);
    clock_t start = clock();
    for (int32_t i = 0; i < 1000000; ++i) {
        UnicodeString appendTo;
        fmt.formatMeasures(
                &measureC, 1, appendTo, fpos, status);
    }
    errln("Took %f", (double) (clock() - start) / CLOCKS_PER_SEC);
    assertSuccess("", status);
*/
}