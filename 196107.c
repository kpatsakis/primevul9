void DecimalQuantity::readLongToBcd(int64_t n) {
    U_ASSERT(n != 0);
    if (n >= 10000000000000000L) {
        ensureCapacity();
        int i = 0;
        for (; n != 0L; n /= 10L, i++) {
            fBCD.bcdBytes.ptr[i] = static_cast<int8_t>(n % 10);
        }
        U_ASSERT(usingBytes);
        scale = 0;
        precision = i;
    } else {
        uint64_t result = 0L;
        int i = 16;
        for (; n != 0L; n /= 10L, i--) {
            result = (result >> 4) + ((n % 10) << 60);
        }
        U_ASSERT(i >= 0);
        U_ASSERT(!usingBytes);
        fBCD.bcdLong = result >> (i * 4);
        scale = 0;
        precision = 16 - i;
    }
}