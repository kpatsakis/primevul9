Formattable::setLong(int32_t l)
{
    dispose();
    fType = kLong;
    fValue.fInt64 = l;
}