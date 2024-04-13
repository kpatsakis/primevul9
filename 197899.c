getCharFromDots (widechar d)
{
  CharOrDots *cdPtr = getCharOrDots (d, 1);
  if (cdPtr)
    return cdPtr->found;
  return ' ';
}