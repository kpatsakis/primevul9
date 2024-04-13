getCharOrDots (widechar c, int m)
{
  CharOrDots *cdPtr;
  TranslationTableOffset bucket;
  unsigned long int makeHash = (unsigned long int) c % HASHNUM;
  if (m == 0)
    bucket = table->charToDots[makeHash];
  else
    bucket = table->dotsToChar[makeHash];
  while (bucket)
    {
      cdPtr = (CharOrDots *) & table->ruleArea[bucket];
      if (cdPtr->lookFor == c)
	return cdPtr;
      bucket = cdPtr->next;
    }
  return NULL;
}