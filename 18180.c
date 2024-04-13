static short ToS(unsigned char *puffer)
{
  return ((short)(puffer[0] | puffer[1] << 8));
}