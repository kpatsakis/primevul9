void DecimalQuantity::appendDigit(int8_t value, int32_t leadingZeros, bool appendAsInteger) {
    U_ASSERT(leadingZeros >= 0);

    // Zero requires special handling to maintain the invariant that the least-significant digit
    // in the BCD is nonzero.
    if (value == 0) {
        if (appendAsInteger && precision != 0) {
            scale += leadingZeros + 1;
        }
        return;
    }

    // Deal with trailing zeros
    if (scale > 0) {
        leadingZeros += scale;
        if (appendAsInteger) {
            scale = 0;
        }
    }

    // Append digit
    shiftLeft(leadingZeros + 1);
    setDigitPos(0, value);

    // Fix scale if in integer mode
    if (appendAsInteger) {
        scale += leadingZeros + 1;
    }
}