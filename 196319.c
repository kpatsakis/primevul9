DecimalQuantity &DecimalQuantity::setToLong(int64_t n) {
    setBcdToZero();
    flags = 0;
    if (n < 0 && n > INT64_MIN) {
        flags |= NEGATIVE_FLAG;
        n = -n;
    }
    if (n != 0) {
        _setToLong(n);
        compact();
    }
    return *this;
}