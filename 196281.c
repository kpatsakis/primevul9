bool DecimalQuantity::isNegative() const {
    return (flags & NEGATIVE_FLAG) != 0;
}