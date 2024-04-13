void DecimalQuantity::roundToIncrement(double roundingIncrement, RoundingMode roundingMode,
                                       int32_t maxFrac, UErrorCode& status) {
    // TODO(13701): Move the nickel check into a higher-level API.
    if (roundingIncrement == 0.05) {
        roundToMagnitude(-2, roundingMode, true, status);
        roundToMagnitude(-maxFrac, roundingMode, false, status);
        return;
    } else if (roundingIncrement == 0.5) {
        roundToMagnitude(-1, roundingMode, true, status);
        roundToMagnitude(-maxFrac, roundingMode, false, status);
        return;
    }
    // TODO(13701): This is innefficient.  Improve?
    // TODO(13701): Should we convert to decNumber instead?
    roundToInfinity();
    double temp = toDouble();
    temp /= roundingIncrement;
    // Use another DecimalQuantity to perform the actual rounding...
    DecimalQuantity dq;
    dq.setToDouble(temp);
    dq.roundToMagnitude(0, roundingMode, status);
    temp = dq.toDouble();
    temp *= roundingIncrement;
    setToDouble(temp);
    // Since we reset the value to a double, we need to specify the rounding boundary
    // in order to get the DecimalQuantity out of approximation mode.
    // NOTE: In Java, we have minMaxFrac, but in C++, the two are differentiated.
    roundToMagnitude(-maxFrac, roundingMode, status);
}