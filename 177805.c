ffi_closure_alloc (size_t size, void **code)
{
  void *ptr;

  if (!code)
    return NULL;

  ptr = dlmalloc (size);

  if (ptr)
    {
      msegmentptr seg = segment_holding (gm, ptr);

      *code = add_segment_exec_offset (ptr, seg);
    }

  return ptr;
}