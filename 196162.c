Formattable::setDouble(double d)
{
    dispose();
    fType = kDouble;
    fValue.fDouble = d;
}