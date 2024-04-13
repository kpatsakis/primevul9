void DecimalQuantity::moveBcdFrom(DecimalQuantity &other) {
    setBcdToZero();
    if (other.usingBytes) {
        usingBytes = true;
        fBCD.bcdBytes.ptr = other.fBCD.bcdBytes.ptr;
        fBCD.bcdBytes.len = other.fBCD.bcdBytes.len;
        // Take ownership away from the old instance:
        other.fBCD.bcdBytes.ptr = nullptr;
        other.usingBytes = false;
    } else {
        fBCD.bcdLong = other.fBCD.bcdLong;
    }
}