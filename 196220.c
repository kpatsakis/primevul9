Formattable::setDate(UDate d)
{
    dispose();
    fType = kDate;
    fValue.fDate = d;
}