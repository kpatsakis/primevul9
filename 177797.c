ffi_closure_free (void *ptr)
{
#if FFI_CLOSURE_FREE_CODE
  msegmentptr seg = segment_holding_code (gm, ptr);

  if (seg)
    ptr = sub_segment_exec_offset (ptr, seg);
#endif

  dlfree (ptr);
}