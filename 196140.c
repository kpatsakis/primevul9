ufmt_getDate(const UFormattable *fmt, UErrorCode *status) {
  const Formattable *obj = Formattable::fromUFormattable(fmt);

  return obj->getDate(*status);
}