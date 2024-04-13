Formattable::setDecimalNumber(StringPiece numberString, UErrorCode &status) {
    if (U_FAILURE(status)) {
        return;
    }
    dispose();

    auto* dq = new DecimalQuantity();
    dq->setToDecNumber(numberString, status);
    adoptDecimalQuantity(dq);

    // Note that we do not hang on to the caller's input string.
    // If we are asked for the string, we will regenerate one from fDecimalQuantity.
}