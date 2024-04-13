static unsigned int UTFCharLength(uint8 lead)
{
  if (lead < 0x80)
    return 1;
  else if ((lead >> 5) == 0x6)
    return 2;
  else if ((lead >> 4) == 0xe)
    return 3;
  else if ((lead >> 3) == 0x1e)
    return 4;
  else
    // Invalid size?
    return 0;
}