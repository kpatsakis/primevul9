void DecimalQuantity::roundToMagnitude(int32_t magnitude, RoundingMode roundingMode, UErrorCode& status) {
    roundToMagnitude(magnitude, roundingMode, false, status);
}