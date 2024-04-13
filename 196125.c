bool DecimalQuantity::adjustMagnitude(int32_t delta) {
    if (precision != 0) {
        // i.e., scale += delta; origDelta += delta
        bool overflow = uprv_add32_overflow(scale, delta, &scale);
        overflow = uprv_add32_overflow(origDelta, delta, &origDelta) || overflow;
        // Make sure that precision + scale won't overflow, either
        int32_t dummy;
        overflow = overflow || uprv_add32_overflow(scale, precision, &dummy);
        return overflow;
    }
    return false;
}