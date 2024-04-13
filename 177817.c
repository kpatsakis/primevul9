ffi_closure_alloc (size_t size, void **code)
{
  if (!code)
    return NULL;

  return *code = malloc (size);
}