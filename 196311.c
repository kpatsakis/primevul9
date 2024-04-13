Formattable::getDouble(UErrorCode& status) const
{
    if (U_FAILURE(status)) {
        return 0;
    }
        
    switch (fType) {
    case Formattable::kLong: 
    case Formattable::kInt64: // loses precision
        return (double)fValue.fInt64;
    case Formattable::kDouble:
        return fValue.fDouble;
    case Formattable::kObject:
        if (fValue.fObject == NULL) {
            status = U_MEMORY_ALLOCATION_ERROR;
            return 0;
        }
        // TODO Later replace this with instanceof call
        if (instanceOfMeasure(fValue.fObject)) {
            return ((const Measure*) fValue.fObject)->
                getNumber().getDouble(status);
        }
        U_FALLTHROUGH;
    default:
        status = U_INVALID_FORMAT_ERROR;
        return 0;
    }
}