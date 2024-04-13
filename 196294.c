DecimalQuantity::~DecimalQuantity() {
    if (usingBytes) {
        uprv_free(fBCD.bcdBytes.ptr);
        fBCD.bcdBytes.ptr = nullptr;
        usingBytes = false;
    }
}