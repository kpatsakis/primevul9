static int gettoalign (size_t len, Header *h, int opt, size_t size) {
  if (size == 0 || opt == 'c') return 0;
  if (size > (size_t)h->align)
    size = h->align;  /* respect max. alignment */
  return (size - (len & (size - 1))) & (size - 1);
}