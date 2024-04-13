ufmt_getLong(UFormattable *fmt, UErrorCode *status) {
  Formattable *obj = Formattable::fromUFormattable(fmt);

  return obj->getLong(*status);
}