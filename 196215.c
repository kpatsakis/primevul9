Formattable::Formattable(const Formattable &source)
     :  UObject(*this)
{
    init();
    *this = source;
}