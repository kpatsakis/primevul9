ufmt_getArrayLength(const UFormattable* fmt, UErrorCode *status) {
  const Formattable *obj = Formattable::fromUFormattable(fmt);

  int32_t count;
  (void)obj->getArray(count, *status);
  return count;
}