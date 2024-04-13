void DecimalQuantity::setDigitPos(int32_t position, int8_t value) {
    U_ASSERT(position >= 0);
    if (usingBytes) {
        ensureCapacity(position + 1);
        fBCD.bcdBytes.ptr[position] = value;
    } else if (position >= 16) {
        switchStorage();
        ensureCapacity(position + 1);
        fBCD.bcdBytes.ptr[position] = value;
    } else {
        int shift = position * 4;
        fBCD.bcdLong = (fBCD.bcdLong & ~(0xfL << shift)) | ((long) value << shift);
    }
}