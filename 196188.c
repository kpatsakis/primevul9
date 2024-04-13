Formattable::getInt64(UErrorCode& status) const
{
    if (U_FAILURE(status)) {
        return 0;
    }
        
    switch (fType) {
    case Formattable::kLong: 
    case Formattable::kInt64: 
        return fValue.fInt64;
    case Formattable::kDouble:
        if (fValue.fDouble > (double)U_INT64_MAX) {
            status = U_INVALID_FORMAT_ERROR;
            return U_INT64_MAX;
        } else if (fValue.fDouble < (double)U_INT64_MIN) {
            status = U_INVALID_FORMAT_ERROR;
            return U_INT64_MIN;
        } else if (fabs(fValue.fDouble) > U_DOUBLE_MAX_EXACT_INT && fDecimalQuantity != NULL) {
            if (fDecimalQuantity->fitsInLong(true)) {
                return fDecimalQuantity->toLong();
            } else {
                // Unexpected
                status = U_INVALID_FORMAT_ERROR;
                return fDecimalQuantity->isNegative() ? U_INT64_MIN : U_INT64_MAX;
            }
        } else {
            return (int64_t)fValue.fDouble;
        } 
    case Formattable::kObject:
        if (fValue.fObject == NULL) {
            status = U_MEMORY_ALLOCATION_ERROR;
            return 0;
        }
        if (instanceOfMeasure(fValue.fObject)) {
            return ((const Measure*) fValue.fObject)->
                getNumber().getInt64(status);
        }
        U_FALLTHROUGH;
    default:
        status = U_INVALID_FORMAT_ERROR;
        return 0;
    }
}