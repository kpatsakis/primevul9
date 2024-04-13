Formattable::Formattable(StringPiece number, UErrorCode &status) {
    init();
    setDecimalNumber(number, status);
}