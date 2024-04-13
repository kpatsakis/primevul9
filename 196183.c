void DecimalQuantity::_setToDoubleFast(double n) {
    isApproximate = true;
    origDouble = n;
    origDelta = 0;

    // Make sure the double is an IEEE 754 double.  If not, fall back to the slow path right now.
    // TODO: Make a fast path for other types of doubles.
    if (!std::numeric_limits<double>::is_iec559) {
        convertToAccurateDouble();
        // Turn off the approximate double flag, since the value is now exact.
        isApproximate = false;
        origDouble = 0.0;
        return;
    }

    // To get the bits from the double, use memcpy, which takes care of endianness.
    uint64_t ieeeBits;
    uprv_memcpy(&ieeeBits, &n, sizeof(n));
    int32_t exponent = static_cast<int32_t>((ieeeBits & 0x7ff0000000000000L) >> 52) - 0x3ff;

    // Not all integers can be represented exactly for exponent > 52
    if (exponent <= 52 && static_cast<int64_t>(n) == n) {
        _setToLong(static_cast<int64_t>(n));
        return;
    }

    // 3.3219... is log2(10)
    auto fracLength = static_cast<int32_t> ((52 - exponent) / 3.32192809489);
    if (fracLength >= 0) {
        int32_t i = fracLength;
        // 1e22 is the largest exact double.
        for (; i >= 22; i -= 22) n *= 1e22;
        n *= DOUBLE_MULTIPLIERS[i];
    } else {
        int32_t i = fracLength;
        // 1e22 is the largest exact double.
        for (; i <= -22; i += 22) n /= 1e22;
        n /= DOUBLE_MULTIPLIERS[-i];
    }
    auto result = static_cast<int64_t>(std::round(n));
    if (result != 0) {
        _setToLong(result);
        scale -= fracLength;
    }
}