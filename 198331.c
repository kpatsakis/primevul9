static unsigned char decode_byte (char ch)
{
  if (ch == 96)
    return 0;
  return ch - 32;
}