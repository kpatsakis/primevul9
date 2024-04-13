ufmt_close(UFormattable *fmt) {
  Formattable *obj = Formattable::fromUFormattable(fmt);

  delete obj;
}