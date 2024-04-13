void NumberFormatTest::TestRounding() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormat *df = (DecimalFormat*)NumberFormat::createCurrencyInstance(Locale::getEnglish(), status);

    if (U_FAILURE(status)) {
        dataerrln("Unable to create decimal formatter. - %s", u_errorName(status));
        return;
    }

    int roundingIncrements[]={1, 2, 5, 20, 50, 100};
    int testValues[]={0, 300};

    for (int j=0; j<2; j++) {
        for (int mode=DecimalFormat::kRoundUp;mode<DecimalFormat::kRoundHalfEven;mode++) {
            df->setRoundingMode((DecimalFormat::ERoundingMode)mode);
            for (int increment=0; increment<6; increment++) {
                double base=testValues[j];
                double rInc=roundingIncrements[increment];
                checkRounding(df, base, 20, rInc);
                rInc=1.000000000/rInc;
                checkRounding(df, base, 20, rInc);
            }
        }
    }
    delete df;
}