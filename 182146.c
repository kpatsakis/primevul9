static char **FontToList(char *font)
{
  char
    **fontlist;

  register char
    *p,
    *q;

  register int
    i;

  unsigned int
    fonts;

  if (font == (char *) NULL)
    return((char **) NULL);
  /*
    Convert string to an ASCII list.
  */
  fonts=1U;
  for (p=font; *p != '\0'; p++)
    if ((*p == ':') || (*p == ';') || (*p == ','))
      fonts++;
  fontlist=(char **) AcquireQuantumMemory((size_t) fonts+1UL,sizeof(*fontlist));
  if (fontlist == (char **) NULL)
    ThrowXWindowFatalException(ResourceLimitError,"MemoryAllocationFailed",
      font);
  p=font;
  for (i=0; i < (int) fonts; i++)
  {
    for (q=p; *q != '\0'; q++)
      if ((*q == ':') || (*q == ';') || (*q == ','))
        break;
    fontlist[i]=(char *) AcquireQuantumMemory((size_t) (q-p)+1UL,
      sizeof(*fontlist[i]));
    if (fontlist[i] == (char *) NULL)
      ThrowXWindowFatalException(ResourceLimitError,"MemoryAllocationFailed",
        font);
    (void) CopyMagickString(fontlist[i],p,(size_t) (q-p+1));
    p=q+1;
  }
  fontlist[i]=(char *) NULL;
  return(fontlist);
}