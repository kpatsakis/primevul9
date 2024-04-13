static i64 readInt64(u8 *p){
#if SQLITE_BYTEORDER==1234 && MSVC_VERSION>=1300
  u64 x;
  memcpy(&x, p, 8);
  return (i64)_byteswap_uint64(x);
#elif SQLITE_BYTEORDER==1234 && GCC_VERSION>=4003000
  u64 x;
  memcpy(&x, p, 8);
  return (i64)__builtin_bswap64(x);
#elif SQLITE_BYTEORDER==4321
  i64 x;
  memcpy(&x, p, 8);
  return x;
#else
  return (i64)(
    (((u64)p[0]) << 56) + 
    (((u64)p[1]) << 48) + 
    (((u64)p[2]) << 40) + 
    (((u64)p[3]) << 32) + 
    (((u64)p[4]) << 24) + 
    (((u64)p[5]) << 16) + 
    (((u64)p[6]) <<  8) + 
    (((u64)p[7]) <<  0)
  );
#endif
}