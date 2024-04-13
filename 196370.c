int32_t DecimalQuantity::fractionCount() const {
    return -getLowerDisplayMagnitude();
}