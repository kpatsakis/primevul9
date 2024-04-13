Formattable::operator==(const Formattable& that) const
{
    int32_t i;

    if (this == &that) return TRUE;

    // Returns FALSE if the data types are different.
    if (fType != that.fType) return FALSE;

    // Compares the actual data values.
    UBool equal = TRUE;
    switch (fType) {
    case kDate:
        equal = (fValue.fDate == that.fValue.fDate);
        break;
    case kDouble:
        equal = (fValue.fDouble == that.fValue.fDouble);
        break;
    case kLong:
    case kInt64:
        equal = (fValue.fInt64 == that.fValue.fInt64);
        break;
    case kString:
        equal = (*(fValue.fString) == *(that.fValue.fString));
        break;
    case kArray:
        if (fValue.fArrayAndCount.fCount != that.fValue.fArrayAndCount.fCount) {
            equal = FALSE;
            break;
        }
        // Checks each element for equality.
        for (i=0; i<fValue.fArrayAndCount.fCount; ++i) {
            if (fValue.fArrayAndCount.fArray[i] != that.fValue.fArrayAndCount.fArray[i]) {
                equal = FALSE;
                break;
            }
        }
        break;
    case kObject:
        if (fValue.fObject == NULL || that.fValue.fObject == NULL) {
            equal = FALSE;
        } else {
            equal = objectEquals(fValue.fObject, that.fValue.fObject);
        }
        break;
    }

    // TODO:  compare digit lists if numeric.
    return equal;
}