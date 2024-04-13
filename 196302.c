ufmt_getType(const UFormattable *fmt, UErrorCode *status) {
  if(U_FAILURE(*status)) {
    return (UFormattableType)UFMT_COUNT;
  }
  const Formattable *obj = Formattable::fromUFormattable(fmt);
  return (UFormattableType)obj->getType();
}