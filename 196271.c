bool DecimalQuantity::isNaN() const {
    return (flags & NAN_FLAG) != 0;
}