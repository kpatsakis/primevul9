void DecimalQuantity::switchStorage() {
    if (usingBytes) {
        // Change from bytes to long
        uint64_t bcdLong = 0L;
        for (int i = precision - 1; i >= 0; i--) {
            bcdLong <<= 4;
            bcdLong |= fBCD.bcdBytes.ptr[i];
        }
        uprv_free(fBCD.bcdBytes.ptr);
        fBCD.bcdBytes.ptr = nullptr;
        fBCD.bcdLong = bcdLong;
        usingBytes = false;
    } else {
        // Change from long to bytes
        // Copy the long into a local variable since it will get munged when we allocate the bytes
        uint64_t bcdLong = fBCD.bcdLong;
        ensureCapacity();
        for (int i = 0; i < precision; i++) {
            fBCD.bcdBytes.ptr[i] = static_cast<int8_t>(bcdLong & 0xf);
            bcdLong >>= 4;
        }
        U_ASSERT(usingBytes);
    }
}