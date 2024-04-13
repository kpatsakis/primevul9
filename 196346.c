void DecimalQuantity::compact() {
    if (usingBytes) {
        int32_t delta = 0;
        for (; delta < precision && fBCD.bcdBytes.ptr[delta] == 0; delta++);
        if (delta == precision) {
            // Number is zero
            setBcdToZero();
            return;
        } else {
            // Remove trailing zeros
            shiftRight(delta);
        }

        // Compute precision
        int32_t leading = precision - 1;
        for (; leading >= 0 && fBCD.bcdBytes.ptr[leading] == 0; leading--);
        precision = leading + 1;

        // Switch storage mechanism if possible
        if (precision <= 16) {
            switchStorage();
        }

    } else {
        if (fBCD.bcdLong == 0L) {
            // Number is zero
            setBcdToZero();
            return;
        }

        // Compact the number (remove trailing zeros)
        // TODO: Use a more efficient algorithm here and below. There is a logarithmic one.
        int32_t delta = 0;
        for (; delta < precision && getDigitPos(delta) == 0; delta++);
        fBCD.bcdLong >>= delta * 4;
        scale += delta;

        // Compute precision
        int32_t leading = precision - 1;
        for (; leading >= 0 && getDigitPos(leading) == 0; leading--);
        precision = leading + 1;
    }
}