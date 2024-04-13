int32_t DecimalQuantity::getUpperDisplayMagnitude() const {
    // If this assertion fails, you need to call roundToInfinity() or some other rounding method.
    // See the comment in the header file explaining the "isApproximate" field.
    U_ASSERT(!isApproximate);

    int32_t magnitude = scale + precision;
    int32_t result = (lReqPos > magnitude) ? lReqPos : (lOptPos < magnitude) ? lOptPos : magnitude;
    return result - 1;
}