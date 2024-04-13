void DecimalQuantity::shiftRight(int32_t numDigits) {
    if (usingBytes) {
        int i = 0;
        for (; i < precision - numDigits; i++) {
            fBCD.bcdBytes.ptr[i] = fBCD.bcdBytes.ptr[i + numDigits];
        }
        for (; i < precision; i++) {
            fBCD.bcdBytes.ptr[i] = 0;
        }
    } else {
        fBCD.bcdLong >>= (numDigits * 4);
    }
    scale += numDigits;
    precision -= numDigits;
}