ufmt_getUChars(UFormattable *fmt, int32_t *len, UErrorCode *status) {
  Formattable *obj = Formattable::fromUFormattable(fmt);

  // avoid bogosity by checking the type first.
  if( obj->getType() != Formattable::kString ) {
    if( U_SUCCESS(*status) ){
      *status = U_INVALID_FORMAT_ERROR;
    }
    return NULL;
  }

  // This should return a valid string
  UnicodeString &str = obj->getString(*status);
  if( U_SUCCESS(*status) && len != NULL ) {
    *len = str.length();
  }
  return str.getTerminatedBuffer();
}