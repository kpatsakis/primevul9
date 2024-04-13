bool DecimalQuantity::isInfinite() const {
    return (flags & INFINITY_FLAG) != 0;
}