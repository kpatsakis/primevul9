uint64_t DecimalQuantity::toFractionLong(bool includeTrailingZeros) const {
    uint64_t result = 0L;
    int32_t magnitude = -1;
    int32_t lowerMagnitude = std::max(scale, rOptPos);
    if (includeTrailingZeros) {
        lowerMagnitude = std::min(lowerMagnitude, rReqPos);
    }
    for (; magnitude >= lowerMagnitude && result <= 1e18L; magnitude--) {
        result = result * 10 + getDigitPos(magnitude - scale);
    }
    // Remove trailing zeros; this can happen during integer overflow cases.
    if (!includeTrailingZeros) {
        while (result > 0 && (result % 10) == 0) {
            result /= 10;
        }
    }
    return result;
}