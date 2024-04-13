MagickExport void *ResetMagickMemory(void *memory,int c,const size_t size)
{
  volatile unsigned char
    *p = memory;

  size_t
    n = size;

  assert(memory != (void *) NULL);
  while (n-- != 0)
  	*p++=(unsigned char) c;
  return(memory);
}