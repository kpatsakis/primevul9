set_multi_byte_cclass(BBuf* mbuf, regex_t* reg)
{
  size_t len;
  void* p;

  len = (size_t )mbuf->used;
  p = xmalloc(len);
  if (IS_NULL(p)) return NULL;

  xmemcpy(p, mbuf->p, len);
  return p;
}