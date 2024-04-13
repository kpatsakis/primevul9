Formattable::getString(UnicodeString& result, UErrorCode& status) const 
{
    if (fType != kString) {
        setError(status, U_INVALID_FORMAT_ERROR);
        result.setToBogus();
    } else {
        if (fValue.fString == NULL) {
            setError(status, U_MEMORY_ALLOCATION_ERROR);
        } else {
            result = *fValue.fString;
        }
    }
    return result;
}