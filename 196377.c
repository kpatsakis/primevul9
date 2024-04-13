Formattable::Formattable(const Formattable* arrayToCopy, int32_t count)
    :   UObject(), fType(kArray)
{
    init();
    fType = kArray;
    fValue.fArrayAndCount.fArray = createArrayCopy(arrayToCopy, count);
    fValue.fArrayAndCount.fCount = count;
}