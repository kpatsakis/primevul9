int8_t DecimalQuantity::getDigit(int32_t magnitude) const {
    // If this assertion fails, you need to call roundToInfinity() or some other rounding method.
    // See the comment at the top of this file explaining the "isApproximate" field.
    U_ASSERT(!isApproximate);

    return getDigitPos(magnitude - scale);
}