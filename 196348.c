Formattable::getArray(int32_t& count, UErrorCode& status) const 
{
    if (fType != kArray) {
        setError(status, U_INVALID_FORMAT_ERROR);
        count = 0;
        return NULL;
    }
    count = fValue.fArrayAndCount.fCount; 
    return fValue.fArrayAndCount.fArray;
}