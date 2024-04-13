int32_t DecimalQuantity::getMagnitude() const {
    U_ASSERT(precision != 0);
    return scale + precision - 1;
}