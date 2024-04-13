static int writeCoord(u8 *p, RtreeCoord *pCoord){
  u32 i;
  assert( ((((char*)p) - (char*)0)&3)==0 );  /* p is always 4-byte aligned */
  assert( sizeof(RtreeCoord)==4 );
  assert( sizeof(u32)==4 );
#if SQLITE_BYTEORDER==1234 && GCC_VERSION>=4003000
  i = __builtin_bswap32(pCoord->u);
  memcpy(p, &i, 4);
#elif SQLITE_BYTEORDER==1234 && MSVC_VERSION>=1300
  i = _byteswap_ulong(pCoord->u);
  memcpy(p, &i, 4);
#elif SQLITE_BYTEORDER==4321
  i = pCoord->u;
  memcpy(p, &i, 4);
#else
  i = pCoord->u;
  p[0] = (i>>24)&0xFF;
  p[1] = (i>>16)&0xFF;
  p[2] = (i>> 8)&0xFF;
  p[3] = (i>> 0)&0xFF;
#endif
  return 4;
}