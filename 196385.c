ufmt_getObject(const UFormattable *fmt, UErrorCode *status) {
  const Formattable *obj = Formattable::fromUFormattable(fmt);

  const void *ret = obj->getObject();
  if( ret==NULL &&
      (obj->getType() != Formattable::kObject) &&
      U_SUCCESS( *status )) {
    *status = U_INVALID_FORMAT_ERROR;
  }
  return ret;
}