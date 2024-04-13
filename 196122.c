Formattable::Formattable(UDate date, ISDATE /*isDate*/)
{
    init();
    fType = kDate;
    fValue.fDate = date;
}