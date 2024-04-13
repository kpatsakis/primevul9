DecimalQuantity& DecimalQuantity::setToDecNum(const DecNum& decnum, UErrorCode& status) {
    setBcdToZero();
    flags = 0;

    _setToDecNum(decnum, status);
    return *this;
}