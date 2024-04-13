Formattable::getString(UErrorCode& status) const 
{
    if (fType != kString) {
        setError(status, U_INVALID_FORMAT_ERROR);
        return *getBogus();
    }
    if (fValue.fString == NULL) {
        setError(status, U_MEMORY_ALLOCATION_ERROR);
        return *getBogus();
    }
    return *fValue.fString;
}