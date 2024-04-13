void DecimalQuantity::_setToInt(int32_t n) {
    if (n == INT32_MIN) {
        readLongToBcd(-static_cast<int64_t>(n));
    } else {
        readIntToBcd(n);
    }
}