void DecimalQuantity::toDecNum(DecNum& output, UErrorCode& status) const {
    // Special handling for zero
    if (precision == 0) {
        output.setTo("0", status);
    }

    // Use the BCD constructor. We need to do a little bit of work to convert, though.
    // The decNumber constructor expects most-significant first, but we store least-significant first.
    MaybeStackArray<uint8_t, 20> ubcd(precision);
    for (int32_t m = 0; m < precision; m++) {
        ubcd[precision - m - 1] = static_cast<uint8_t>(getDigitPos(m));
    }
    output.setTo(ubcd.getAlias(), precision, scale, isNegative(), status);
}