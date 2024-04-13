ufmt_getDecNumChars(UFormattable *fmt, int32_t *len, UErrorCode *status) {
  if(U_FAILURE(*status)) {
    return "";
  }
  Formattable *obj = Formattable::fromUFormattable(fmt);
  CharString *charString = obj->internalGetCharString(*status);
  if(U_FAILURE(*status)) {
    return "";
  }
  if(charString == NULL) {
    *status = U_MEMORY_ALLOCATION_ERROR;
    return "";
  } else {
    if(len!=NULL) {
      *len = charString->length();
    }
    return charString->data();
  }
}