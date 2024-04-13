void DecimalQuantity::multiplyBy(const DecNum& multiplicand, UErrorCode& status) {
    if (isInfinite() || isZero() || isNaN()) {
        return;
    }
    // Convert to DecNum, multiply, and convert back.
    DecNum decnum;
    toDecNum(decnum, status);
    if (U_FAILURE(status)) { return; }
    decnum.multiplyBy(multiplicand, status);
    if (U_FAILURE(status)) { return; }
    setToDecNum(decnum, status);
}