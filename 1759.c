void optimized_ops_prefetch_write_l1_keep(const T* ptr) {
#ifdef __GNUC__
  // builtin offered by GCC-compatible compilers including clang
  __builtin_prefetch(ptr, /* 1 means write */ 1, /* 3 means high locality */ 3);
#else
  (void)ptr;
#endif
}