liblouis_allocMem (AllocBuf buffer, int srcmax, int destmax)
{
  if (srcmax < 1024)
    srcmax = 1024;
  if (destmax < 1024)
    destmax = 1024;
  switch (buffer)
    {
    case alloc_typebuf:
      if (destmax > sizeTypebuf)
	{
	  if (typebuf != NULL)
	    free (typebuf);
	  typebuf = malloc ((destmax + 4) * sizeof (unsigned short));
	  if (!typebuf)
	    outOfMemory ();
	  sizeTypebuf = destmax;
	}
      return typebuf;
    case alloc_destSpacing:
      if (destmax > sizeDestSpacing)
	{
	  if (destSpacing != NULL)
	    free (destSpacing);
	  destSpacing = malloc (destmax + 4);
	  if (!destSpacing)
	    outOfMemory ();
	  sizeDestSpacing = destmax;
	}
      return destSpacing;
    case alloc_passbuf1:
      if (destmax > sizePassbuf1)
	{
	  if (passbuf1 != NULL)
	    free (passbuf1);
	  passbuf1 = malloc ((destmax + 4) * CHARSIZE);
	  if (!passbuf1)
	    outOfMemory ();
	  sizePassbuf1 = destmax;
	}
      return passbuf1;
    case alloc_passbuf2:
      if (destmax > sizePassbuf2)
	{
	  if (passbuf2 != NULL)
	    free (passbuf2);
	  passbuf2 = malloc ((destmax + 4) * CHARSIZE);
	  if (!passbuf2)
	    outOfMemory ();
	  sizePassbuf2 = destmax;
	}
      return passbuf2;
    case alloc_srcMapping:
      {
	int mapSize;
	if (srcmax >= destmax)
	  mapSize = srcmax;
	else
	  mapSize = destmax;
	if (mapSize > sizeSrcMapping)
	  {
	    if (srcMapping != NULL)
	      free (srcMapping);
	    srcMapping = malloc ((mapSize + 4) * sizeof (int));
	    if (!srcMapping)
	      outOfMemory ();
	    sizeSrcMapping = mapSize;
	  }
      }
      return srcMapping;
    case alloc_prevSrcMapping:
      {
	int mapSize;
	if (srcmax >= destmax)
	  mapSize = srcmax;
	else
	  mapSize = destmax;
	if (mapSize > sizePrevSrcMapping)
	  {
	    if (prevSrcMapping != NULL)
	      free (prevSrcMapping);
	    prevSrcMapping = malloc ((mapSize + 4) * sizeof (int));
	    if (!prevSrcMapping)
	      outOfMemory ();
	    sizePrevSrcMapping = mapSize;
	  }
      }
      return prevSrcMapping;
    default:
      return NULL;
    }
}