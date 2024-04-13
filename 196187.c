UnicodeString DecimalQuantity::toString() const {
    MaybeStackArray<char, 30> digits(precision + 1);
    for (int32_t i = 0; i < precision; i++) {
        digits[i] = getDigitPos(precision - i - 1) + '0';
    }
    digits[precision] = 0; // terminate buffer
    char buffer8[100];
    snprintf(
            buffer8,
            sizeof(buffer8),
            "<DecimalQuantity %d:%d:%d:%d %s %s%s%s%d>",
            (lOptPos > 999 ? 999 : lOptPos),
            lReqPos,
            rReqPos,
            (rOptPos < -999 ? -999 : rOptPos),
            (usingBytes ? "bytes" : "long"),
            (isNegative() ? "-" : ""),
            (precision == 0 ? "0" : digits.getAlias()),
            "E",
            scale);
    return UnicodeString(buffer8, -1, US_INV);
}