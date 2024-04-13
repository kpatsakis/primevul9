Formattable::setInt64(int64_t ll)
{
    dispose();
    fType = kInt64;
    fValue.fInt64 = ll;
}