compileSwapDots (FileInfo * nested, CharsString * source, CharsString * dest)
{
  int k = 0;
  int kk = 0;
  CharsString dotsSource;
  CharsString dotsDest;
  dest->length = 0;
  dotsSource.length = 0;
  while (k <= source->length)
    {
      if (source->chars[k] != ',' && k != source->length)
	dotsSource.chars[dotsSource.length++] = source->chars[k];
      else
	{
	  if (!parseDots (nested, &dotsDest, &dotsSource))
	    return 0;
	  dest->chars[dest->length++] = dotsDest.length + 1;
	  for (kk = 0; kk < dotsDest.length; kk++)
	    dest->chars[dest->length++] = dotsDest.chars[kk];
	  dotsSource.length = 0;
	}
      k++;
    }
  return 1;
}