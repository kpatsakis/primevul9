void CLASS sony_decrypt (unsigned *data, int len, int start, int key)
{
#ifndef LIBRAW_NOTHREADS
#define pad tls->sony_decrypt.pad
#define p   tls->sony_decrypt.p
#else
  static unsigned pad[128], p;
#endif
  if (start) {
    for (p=0; p < 4; p++)
      pad[p] = key = key * 48828125 + 1;
    pad[3] = pad[3] << 1 | (pad[0]^pad[2]) >> 31;
    for (p=4; p < 127; p++)
      pad[p] = (pad[p-4]^pad[p-2]) << 1 | (pad[p-3]^pad[p-1]) >> 31;
    for (p=0; p < 127; p++)
      pad[p] = htonl(pad[p]);
  }
  while (len--)
    {
      *data++ ^= pad[p & 127] = pad[(p+1) & 127] ^ pad[(p+65) & 127];
      p++;     
    }
#ifndef LIBRAW_NOTHREADS
#undef pad
#undef p
#endif
}