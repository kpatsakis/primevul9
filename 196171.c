void DecimalQuantity::readDecNumberToBcd(const DecNum& decnum) {
    const decNumber* dn = decnum.getRawDecNumber();
    if (dn->digits > 16) {
        ensureCapacity(dn->digits);
        for (int32_t i = 0; i < dn->digits; i++) {
            fBCD.bcdBytes.ptr[i] = dn->lsu[i];
        }
    } else {
        uint64_t result = 0L;
        for (int32_t i = 0; i < dn->digits; i++) {
            result |= static_cast<uint64_t>(dn->lsu[i]) << (4 * i);
        }
        fBCD.bcdLong = result;
    }
    scale = dn->exponent;
    precision = dn->digits;
}