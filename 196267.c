Formattable::isNumeric() const {
    switch (fType) {
    case kDouble:
    case kLong:
    case kInt64:
        return TRUE;
    default:
        return FALSE;
    }
}