int32_t DecimalQuantity::fractionCountWithoutTrailingZeros() const {
    return -scale > 0 ? -scale : 0;  // max(-scale, 0)
}