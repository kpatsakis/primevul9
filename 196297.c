void Formattable::dispose()
{
    // Deletes the data value if necessary.
    switch (fType) {
    case kString:
        delete fValue.fString;
        break;
    case kArray:
        delete[] fValue.fArrayAndCount.fArray;
        break;
    case kObject:
        delete fValue.fObject;
        break;
    default:
        break;
    }

    fType = kLong;
    fValue.fInt64 = 0;

    delete fDecimalStr;
    fDecimalStr = NULL;

    delete fDecimalQuantity;
    fDecimalQuantity = NULL;
}