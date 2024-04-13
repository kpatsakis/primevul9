ufmt_getArrayItemByIndex(UFormattable* fmt, int32_t n, UErrorCode *status) {
  Formattable *obj = Formattable::fromUFormattable(fmt);
  int32_t count;
  (void)obj->getArray(count, *status);
  if(U_FAILURE(*status)) {
    return NULL;
  } else if(n<0 || n>=count) {
    setError(*status, U_INDEX_OUTOFBOUNDS_ERROR);
    return NULL;
  } else {
    return (*obj)[n].toUFormattable(); // returns non-const Formattable
  }
}