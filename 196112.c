void DecimalQuantity::shiftLeft(int32_t numDigits) {
    if (!usingBytes && precision + numDigits > 16) {
        switchStorage();
    }
    if (usingBytes) {
        ensureCapacity(precision + numDigits);
        int i = precision + numDigits - 1;
        for (; i >= numDigits; i--) {
            fBCD.bcdBytes.ptr[i] = fBCD.bcdBytes.ptr[i - numDigits];
        }
        for (; i >= 0; i--) {
            fBCD.bcdBytes.ptr[i] = 0;
        }
    } else {
        fBCD.bcdLong <<= (numDigits * 4);
    }
    scale -= numDigits;
    precision += numDigits;
}