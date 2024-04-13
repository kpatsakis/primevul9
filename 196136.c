void DecimalQuantity::_setToDecNum(const DecNum& decnum, UErrorCode& status) {
    if (U_FAILURE(status)) { return; }
    if (decnum.isNegative()) {
        flags |= NEGATIVE_FLAG;
    }
    if (!decnum.isZero()) {
        readDecNumberToBcd(decnum);
        compact();
    }
}