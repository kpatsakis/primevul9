getDotsForChar (widechar c)
{
  CharOrDots *cdPtr = getCharOrDots (c, 0);
  if (cdPtr)
    return cdPtr->found;
  return B16;
}