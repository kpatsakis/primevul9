void DecimalQuantity::readIntToBcd(int32_t n) {
    U_ASSERT(n != 0);
    // ints always fit inside the long implementation.
    uint64_t result = 0L;
    int i = 16;
    for (; n != 0; n /= 10, i--) {
        result = (result >> 4) + ((static_cast<uint64_t>(n) % 10) << 60);
    }
    U_ASSERT(!usingBytes);
    fBCD.bcdLong = result >> (i * 4);
    scale = 0;
    precision = 16 - i;
}