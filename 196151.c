Formattable::Formattable(const UnicodeString& stringToCopy)
{
    init();
    fType = kString;
    fValue.fString = new UnicodeString(stringToCopy);
}