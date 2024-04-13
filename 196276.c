Formattable::getLong(UErrorCode& status) const
{
    if (U_FAILURE(status)) {
        return 0;
    }
        
    switch (fType) {
    case Formattable::kLong: 
        return (int32_t)fValue.fInt64;
    case Formattable::kInt64:
        if (fValue.fInt64 > INT32_MAX) {
            status = U_INVALID_FORMAT_ERROR;
            return INT32_MAX;
        } else if (fValue.fInt64 < INT32_MIN) {
            status = U_INVALID_FORMAT_ERROR;
            return INT32_MIN;
        } else {
            return (int32_t)fValue.fInt64;
        }
    case Formattable::kDouble:
        if (fValue.fDouble > INT32_MAX) {
            status = U_INVALID_FORMAT_ERROR;
            return INT32_MAX;
        } else if (fValue.fDouble < INT32_MIN) {
            status = U_INVALID_FORMAT_ERROR;
            return INT32_MIN;
        } else {
            return (int32_t)fValue.fDouble; // loses fraction
        }
    case Formattable::kObject:
        if (fValue.fObject == NULL) {
            status = U_MEMORY_ALLOCATION_ERROR;
            return 0;
        }
        // TODO Later replace this with instanceof call
        if (instanceOfMeasure(fValue.fObject)) {
            return ((const Measure*) fValue.fObject)->
                getNumber().getLong(status);
        }
        U_FALLTHROUGH;
    default:
        status = U_INVALID_FORMAT_ERROR;
        return 0;
    }
}