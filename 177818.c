ffi_clear_cache (void *start, void *end)
{
#if defined (__clang__) && defined (__APPLE__)
  sys_icache_invalidate (start, (char *)end - (char *)start);
#elif defined (__GNUC__)
  __builtin___clear_cache (start, end);
#else
#error "Missing builtin to flush instruction cache"
#endif
}