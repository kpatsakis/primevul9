int8_t DecimalQuantity::signum() const {
    return isNegative() ? -1 : isZero() ? 0 : 1;
}