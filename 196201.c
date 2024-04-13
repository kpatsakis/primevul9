void DecimalQuantity::setBcdToZero() {
    if (usingBytes) {
        uprv_free(fBCD.bcdBytes.ptr);
        fBCD.bcdBytes.ptr = nullptr;
        usingBytes = false;
    }
    fBCD.bcdLong = 0L;
    scale = 0;
    precision = 0;
    isApproximate = false;
    origDouble = 0;
    origDelta = 0;
}