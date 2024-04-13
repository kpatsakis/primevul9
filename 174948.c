ToL (const guchar *puffer)
{
  return (puffer[0] | puffer[1] << 8 | puffer[2] << 16 | puffer[3] << 24);
}