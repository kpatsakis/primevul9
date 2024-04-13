Formattable::Formattable(UnicodeString* stringToAdopt)
{
    init();
    fType = kString;
    fValue.fString = stringToAdopt;
}