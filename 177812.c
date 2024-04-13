ffi_prep_go_closure (ffi_go_closure *closure, ffi_cif* cif,
                     void (*fun)(ffi_cif*,void*,void**,void*))
{
  void (*start)(void);

  if (cif->abi != FFI_SYSV)
    return FFI_BAD_ABI;

  if (cif->flags & AARCH64_FLAG_ARG_V)
    start = ffi_go_closure_SYSV_V;
  else
    start = ffi_go_closure_SYSV;

  closure->tramp = start;
  closure->cif = cif;
  closure->fun = fun;

  return FFI_OK;
}