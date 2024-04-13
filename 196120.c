double DecimalQuantity::getPluralOperand(PluralOperand operand) const {
    // If this assertion fails, you need to call roundToInfinity() or some other rounding method.
    // See the comment at the top of this file explaining the "isApproximate" field.
    U_ASSERT(!isApproximate);

    switch (operand) {
        case PLURAL_OPERAND_I:
            // Invert the negative sign if necessary
            return static_cast<double>(isNegative() ? -toLong(true) : toLong(true));
        case PLURAL_OPERAND_F:
            return static_cast<double>(toFractionLong(true));
        case PLURAL_OPERAND_T:
            return static_cast<double>(toFractionLong(false));
        case PLURAL_OPERAND_V:
            return fractionCount();
        case PLURAL_OPERAND_W:
            return fractionCountWithoutTrailingZeros();
        default:
            return std::abs(toDouble());
    }
}