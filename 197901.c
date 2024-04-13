hyphenStringHash (const CharsString * s)
{
  int k;
  unsigned int h = 0, g;
  for (k = 0; k < s->length; k++)
    {
      h = (h << 4) + s->chars[k];
      if ((g = h & 0xf0000000))
	{
	  h = h ^ (g >> 24);
	  h = h ^ g;
	}
    }
  return h;
}