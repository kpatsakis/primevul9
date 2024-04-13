void DecimalQuantity::divideBy(const DecNum& divisor, UErrorCode& status) {
    if (isInfinite() || isZero() || isNaN()) {
        return;
    }
    // Convert to DecNum, multiply, and convert back.
    DecNum decnum;
    toDecNum(decnum, status);
    if (U_FAILURE(status)) { return; }
    decnum.divideBy(divisor, status);
    if (U_FAILURE(status)) { return; }
    setToDecNum(decnum, status);
}