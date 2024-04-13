ufmt_getDouble(UFormattable *fmt, UErrorCode *status) {
  Formattable *obj = Formattable::fromUFormattable(fmt);

  return obj->getDouble(*status);
}