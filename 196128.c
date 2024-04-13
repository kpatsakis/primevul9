void NumberFormatTest::Test10419RoundingWith0FractionDigits() {
    const Test10419Data items[] = {
        { DecimalFormat::kRoundCeiling, 1.488,  "2"},
        { DecimalFormat::kRoundDown, 1.588,  "1"},
        { DecimalFormat::kRoundFloor, 1.888,  "1"},
        { DecimalFormat::kRoundHalfDown, 1.5,  "1"},
        { DecimalFormat::kRoundHalfEven, 2.5,  "2"},
        { DecimalFormat::kRoundHalfUp, 2.5,  "3"},
        { DecimalFormat::kRoundUp, 1.5,  "2"},
    };
    UErrorCode status = U_ZERO_ERROR;
    LocalPointer<DecimalFormat> decfmt((DecimalFormat *) NumberFormat::createInstance(Locale("en_US"), status));
    if (U_FAILURE(status)) {
        dataerrln("Failure creating DecimalFormat %s", u_errorName(status));
        return;
    }
    for (int32_t i = 0; i < UPRV_LENGTHOF(items); ++i) {
        decfmt->setRoundingMode(items[i].mode);
        decfmt->setMaximumFractionDigits(0);
        UnicodeString actual;
        if (items[i].expected != decfmt->format(items[i].value, actual)) {
            errln("Expected " + items[i].expected + ", got " + actual);
        }
    }
}