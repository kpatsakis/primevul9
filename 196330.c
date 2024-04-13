void DecimalQuantity::roundToNickel(int32_t magnitude, RoundingMode roundingMode, UErrorCode& status) {
    roundToMagnitude(magnitude, roundingMode, true, status);
}