DecimalQuantity &DecimalQuantity::setToDouble(double n) {
    setBcdToZero();
    flags = 0;
    // signbit() from <math.h> handles +0.0 vs -0.0
    if (std::signbit(n)) {
        flags |= NEGATIVE_FLAG;
        n = -n;
    }
    if (std::isnan(n) != 0) {
        flags |= NAN_FLAG;
    } else if (std::isfinite(n) == 0) {
        flags |= INFINITY_FLAG;
    } else if (n != 0) {
        _setToDoubleFast(n);
        compact();
    }
    return *this;
}