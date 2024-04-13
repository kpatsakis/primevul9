void DecimalQuantity::_setToLong(int64_t n) {
    if (n == INT64_MIN) {
        DecNum decnum;
        UErrorCode localStatus = U_ZERO_ERROR;
        decnum.setTo("9.223372036854775808E+18", localStatus);
        if (U_FAILURE(localStatus)) { return; } // unexpected
        flags |= NEGATIVE_FLAG;
        readDecNumberToBcd(decnum);
    } else if (n <= INT32_MAX) {
        readIntToBcd(static_cast<int32_t>(n));
    } else {
        readLongToBcd(n);
    }
}