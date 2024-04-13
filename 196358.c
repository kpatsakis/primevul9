Formattable::adoptString(UnicodeString* stringToAdopt)
{
    dispose();
    fType = kString;
    fValue.fString = stringToAdopt;
}