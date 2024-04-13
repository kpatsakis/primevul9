void DecimalQuantity::setIntegerLength(int32_t minInt, int32_t maxInt) {
    // Validation should happen outside of DecimalQuantity, e.g., in the Precision class.
    U_ASSERT(minInt >= 0);
    U_ASSERT(maxInt >= minInt);

    // Special behavior: do not set minInt to be less than what is already set.
    // This is so significant digits rounding can set the integer length.
    if (minInt < lReqPos) {
        minInt = lReqPos;
    }

    // Save values into internal state
    // Negation is safe for minFrac/maxFrac because -Integer.MAX_VALUE > Integer.MIN_VALUE
    lOptPos = maxInt;
    lReqPos = minInt;
}