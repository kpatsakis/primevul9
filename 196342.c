Formattable::populateDecimalQuantity(number::impl::DecimalQuantity& output, UErrorCode& status) const {
    if (fDecimalQuantity != nullptr) {
        output = *fDecimalQuantity;
        return;
    }

    switch (fType) {
        case kDouble:
            output.setToDouble(this->getDouble());
            output.roundToInfinity();
            break;
        case kLong:
            output.setToInt(this->getLong());
            break;
        case kInt64:
            output.setToLong(this->getInt64());
            break;
        default:
            // The formattable's value is not a numeric type.
            status = U_INVALID_STATE_ERROR;
    }
}