int8_t DecimalQuantity::getDigitPos(int32_t position) const {
    if (usingBytes) {
        if (position < 0 || position >= precision) { return 0; }
        return fBCD.bcdBytes.ptr[position];
    } else {
        if (position < 0 || position >= 16) { return 0; }
        return (int8_t) ((fBCD.bcdLong >> (position * 4)) & 0xf);
    }
}