double DecimalQuantity::toDouble() const {
    // If this assertion fails, you need to call roundToInfinity() or some other rounding method.
    // See the comment in the header file explaining the "isApproximate" field.
    U_ASSERT(!isApproximate);

    if (isNaN()) {
        return NAN;
    } else if (isInfinite()) {
        return isNegative() ? -INFINITY : INFINITY;
    }

    // We are processing well-formed input, so we don't need any special options to StringToDoubleConverter.
    StringToDoubleConverter converter(0, 0, 0, "", "");
    UnicodeString numberString = this->toScientificString();
    int32_t count;
    return converter.StringToDouble(
            reinterpret_cast<const uint16_t*>(numberString.getBuffer()),
            numberString.length(),
            &count);
}