ffi_call (ffi_cif *cif, void (*fn) (void), void *rvalue, void **avalue)
{
  ffi_call_int (cif, fn, rvalue, avalue, NULL);
}