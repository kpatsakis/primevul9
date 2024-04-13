ufmt_open(UErrorCode *status) {
  if( U_FAILURE(*status) ) {
    return NULL;
  }
  UFormattable *fmt = (new Formattable())->toUFormattable();

  if( fmt == NULL ) {
    *status = U_MEMORY_ALLOCATION_ERROR;
  }
  return fmt;
}