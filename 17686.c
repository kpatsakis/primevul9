static void print_vtable(vtableEntry* start, int len, outputStream* st) {
  return print_vtable(reinterpret_cast<intptr_t*>(start), len, st);
}