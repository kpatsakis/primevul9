static void print_vtable(intptr_t* start, int len, outputStream* st) {
  for (int i = 0; i < len; i++) {
    intptr_t e = start[i];
    st->print("%d : " INTPTR_FORMAT, i, e);
    if (MetaspaceObj::is_valid((Metadata*)e)) {
      st->print(" ");
      ((Metadata*)e)->print_value_on(st);
    }
    st->cr();
  }
}