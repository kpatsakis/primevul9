int32_t DecimalQuantity::getLowerDisplayMagnitude() const {
    // If this assertion fails, you need to call roundToInfinity() or some other rounding method.
    // See the comment in the header file explaining the "isApproximate" field.
    U_ASSERT(!isApproximate);

    int32_t magnitude = scale;
    int32_t result = (rReqPos < magnitude) ? rReqPos : (rOptPos > magnitude) ? rOptPos : magnitude;
    return result;
}