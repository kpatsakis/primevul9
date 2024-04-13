ufmt_isNumeric(const UFormattable *fmt) {
  const Formattable *obj = Formattable::fromUFormattable(fmt);
  return obj->isNumeric();
}