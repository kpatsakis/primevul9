void DecimalQuantity::setFractionLength(int32_t minFrac, int32_t maxFrac) {
    // Validation should happen outside of DecimalQuantity, e.g., in the Precision class.
    U_ASSERT(minFrac >= 0);
    U_ASSERT(maxFrac >= minFrac);

    // Save values into internal state
    // Negation is safe for minFrac/maxFrac because -Integer.MAX_VALUE > Integer.MIN_VALUE
    rReqPos = -minFrac;
    rOptPos = -maxFrac;
}