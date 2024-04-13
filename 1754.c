void optimized_ops_preload_l1_stream(const T* ptr) {
#ifdef __GNUC__
  // builtin offered by GCC-compatible compilers including clang
  __builtin_prefetch(ptr, /* 0 means read */ 0, /* 0 means no locality */ 0);
#else
  (void)ptr;
#endif
}