const char16_t* DecimalQuantity::checkHealth() const {
    if (usingBytes) {
        if (precision == 0) { return u"Zero precision but we are in byte mode"; }
        int32_t capacity = fBCD.bcdBytes.len;
        if (precision > capacity) { return u"Precision exceeds length of byte array"; }
        if (getDigitPos(precision - 1) == 0) { return u"Most significant digit is zero in byte mode"; }
        if (getDigitPos(0) == 0) { return u"Least significant digit is zero in long mode"; }
        for (int i = 0; i < precision; i++) {
            if (getDigitPos(i) >= 10) { return u"Digit exceeding 10 in byte array"; }
            if (getDigitPos(i) < 0) { return u"Digit below 0 in byte array"; }
        }
        for (int i = precision; i < capacity; i++) {
            if (getDigitPos(i) != 0) { return u"Nonzero digits outside of range in byte array"; }
        }
    } else {
        if (precision == 0 && fBCD.bcdLong != 0) {
            return u"Value in bcdLong even though precision is zero";
        }
        if (precision > 16) { return u"Precision exceeds length of long"; }
        if (precision != 0 && getDigitPos(precision - 1) == 0) {
            return u"Most significant digit is zero in long mode";
        }
        if (precision != 0 && getDigitPos(0) == 0) {
            return u"Least significant digit is zero in long mode";
        }
        for (int i = 0; i < precision; i++) {
            if (getDigitPos(i) >= 10) { return u"Digit exceeding 10 in long"; }
            if (getDigitPos(i) < 0) { return u"Digit below 0 in long (?!)"; }
        }
        for (int i = precision; i < 16; i++) {
            if (getDigitPos(i) != 0) { return u"Nonzero digits outside of range in long"; }
        }
    }

    // No error
    return nullptr;
}