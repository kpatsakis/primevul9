void DecimalQuantity::copyBcdFrom(const DecimalQuantity &other) {
    setBcdToZero();
    if (other.usingBytes) {
        ensureCapacity(other.precision);
        uprv_memcpy(fBCD.bcdBytes.ptr, other.fBCD.bcdBytes.ptr, other.precision * sizeof(int8_t));
    } else {
        fBCD.bcdLong = other.fBCD.bcdLong;
    }
}