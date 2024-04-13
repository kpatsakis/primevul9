ufmt_getInt64(UFormattable *fmt, UErrorCode *status) {
  Formattable *obj = Formattable::fromUFormattable(fmt);
  return obj->getInt64(*status);
}